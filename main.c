#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#define N 2048u
#define WALK_N 8192u
#define REPEATS 200u
#define TRIALS 5u
static uint32_t a[N], b[N];
static volatile uint32_t c[N];
static volatile uint32_t walk_data[WALK_N];
static volatile uint32_t sink;
__attribute__((noinline))
static void vector_add_once(void) {
for (unsigned i = 0; i < N; ++i) c[i] = a[i] + b[i];
}
static int64_t time_vector_add(void) {
int64_t t0 = esp_timer_get_time();
for (unsigned r = 0; r < REPEATS; ++r) vector_add_once();
int64_t us = esp_timer_get_time() - t0;
sink ^= c[0];
return us;
}
__attribute__((noinline))
static int64_t time_walk(unsigned step) {
uint32_t sum = 0;
int64_t t0 = esp_timer_get_time();
for (unsigned r = 0; r < REPEATS; ++r) {
unsigned idx = 0;
for (unsigned i = 0; i < WALK_N; ++i) {
sum += walk_data[idx];
idx = (idx + step) & (WALK_N - 1u);
}
}
int64_t us = esp_timer_get_time() - t0;
sink ^= sum;
return us;
}
void app_main(void) {
for (unsigned i = 0; i < N; ++i) { a[i] = i; b[i] = 2u * i; }
for (unsigned i = 0; i < WALK_N; ++i)
walk_data[i] = (i * 7u) ^ 0xA5A5u;
vector_add_once();
printf("Check c[7]=%lu (expected 21)\n", (unsigned long)c[7]);
if (c[7] != 21u) { printf("Incorrect result!\n"); return; }
(void)time_vector_add(); (void)time_walk(1u);
for (unsigned k = 0; k < TRIALS; ++k) {
int64_t tv = time_vector_add();
int64_t ts = time_walk(65u); //for step 1 make it 1u else 65 u
printf("Trial %u: vector=%" PRId64 " us, step65=%" PRId64 " us\n",
k + 1, tv, ts);
vTaskDelay(pdMS_TO_TICKS(100));
}
printf("sink=%lu\n", (unsigned long)sink);
}
