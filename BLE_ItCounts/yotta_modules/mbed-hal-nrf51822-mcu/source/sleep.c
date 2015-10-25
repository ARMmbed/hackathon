/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "sleep_api.h"
#include "cmsis.h"
#include "mbed_interface.h"
#include "nrf_soc.h"

void mbed_enter_sleep(sleep_t *obj)
{
    (void)obj;

    // ensure debug is disconnected if semihost is enabled....
    NRF_POWER->TASKS_LOWPWR = 1;

    SCB->SCR |= SCB_SCR_SEVONPEND_Msk; /* send an event when an interrupt is pending.
                                        * This helps with the wakeup from the following app_evt_wait(). */
    sd_app_evt_wait();
}

void mbed_exit_sleep(sleep_t *obj)
{
    (void)obj;
}
