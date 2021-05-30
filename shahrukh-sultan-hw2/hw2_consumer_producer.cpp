using namespace std;

#include <pthread.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <random>
#include <thread>

#pragma warning( disable : 4091 )           // disable ['keyword' : ignored on left of 'type' when no variable is declared] warning 

struct thread_primitives {
    pthread_mutex_t sv_mutex = PTHREAD_MUTEX_INITIALIZER;        // sv_mutex =shared variable mutex
    pthread_mutex_t producer_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t produced_value_condition = PTHREAD_COND_INITIALIZER;
    pthread_cond_t consumed_value_condition = PTHREAD_COND_INITIALIZER;
};

thread; int partial_sum = 0;                 //thread; std::thread standard defination

bool is_running = true;

unsigned int c_n;               //c_n =number of consumers
int consumer_sleep_upper_limit = 0;

pthread_barrier_t consumers_barrier;
std::vector<pthread_t> consumers;

thread_primitives primitives;

int get_random_integer(int l, int u) {                //l= lower limit, u=uper_limit
    std::random_device dev;
    std::mt19937 generator(dev());
    std::uniform_int_distribution<int> distribution(l, u);
    return distribution(generator);
}

void* producer_routine(void* arg) {
    // Wait for consumer to start
    pthread_barrier_wait(&consumers_barrier);

    // Read primitives, loop through each value and update the value, notify consumer, wait for consumer to process
    int* shared_variable_pointer = static_cast<int*>(arg);
    std::string input;
    std::getline(std::cin, input);
    std::stringstream ss(input);

    int n;
    while (ss >> n) {
        pthread_mutex_lock(&primitives.sv_mutex);
        *shared_variable_pointer = n;
        pthread_cond_signal(&primitives.produced_value_condition);
        pthread_mutex_unlock(&primitives.sv_mutex);

        pthread_mutex_lock(&primitives.producer_mutex);
        while (*shared_variable_pointer != 0) {
            pthread_cond_wait(&primitives.consumed_value_condition, &primitives.producer_mutex);
        }
        pthread_mutex_unlock(&primitives.producer_mutex);
    }

    pthread_mutex_lock(&primitives.sv_mutex);
    is_running = false;
    pthread_cond_broadcast(&primitives.produced_value_condition);
    pthread_mutex_unlock(&primitives.sv_mutex);

    return nullptr;
}

void* consumer_routine(void* arg) {                                           // notify about start
                                                                              // for every update issued by producer, read the value and add to sum
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
    pthread_barrier_wait(&consumers_barrier);
  
    // return pointer to result (for particular consumer)
  
    int* shared_variable_pointer = static_cast<int*>(arg);
    while (is_running) {
        pthread_mutex_lock(&primitives.sv_mutex);

        while (is_running && *shared_variable_pointer == 0) {
            pthread_cond_wait(&primitives.produced_value_condition, &primitives.sv_mutex);
        }

        partial_sum += *shared_variable_pointer;
        *shared_variable_pointer = 0;

        pthread_mutex_lock(&primitives.producer_mutex);
        pthread_cond_signal(&primitives.consumed_value_condition);
        pthread_mutex_unlock(&primitives.producer_mutex);

        pthread_mutex_unlock(&primitives.sv_mutex);

        std::this_thread::sleep_for(std::chrono::milliseconds(get_random_integer(0, consumer_sleep_upper_limit)));
    }

    return &partial_sum;
}

void* consumer_interruptor_routine(void* arg) {
    // wait for consumers to start
    pthread_barrier_wait(&consumers_barrier);

    // interrupt random consumer while producer is running
    while (is_running) {
        pthread_cancel(consumers[get_random_integer(0, c_n - 1)]);
    }

    return nullptr;
}

int run_threads() {
    // start N threads and wait until they're done
    // return aggregated sum of values
    pthread_barrier_init(&consumers_barrier, nullptr, c_n
        + 2);

    int updates_variable = 0;

    pthread_t producer, interrupter;
    pthread_create(&producer, nullptr, producer_routine, &updates_variable);
    consumers = std::vector<pthread_t>(c_n);
    for (auto& consumer : consumers) {
        pthread_create(&consumer, nullptr, consumer_routine, &updates_variable);
    }
    pthread_create(&interrupter, nullptr, consumer_interruptor_routine, nullptr);

    pthread_join(producer, nullptr);
    pthread_join(interrupter, nullptr);

    int total_sum = 0;
    for (auto& consumer : consumers) {
        int* return_value;
        pthread_join(consumer, (void**)&return_value);
        total_sum += *return_value;
    }

    return total_sum;
}

int main(int argc, char* argv[]) {
    c_n = static_cast<unsigned int>(std::stoi(argv[1]));
    consumer_sleep_upper_limit = std::stoi(argv[2]);

    std::cout << run_threads() << std::endl;
    return 0;
}
