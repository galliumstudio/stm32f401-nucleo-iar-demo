/*******************************************************************************
 * Copyright (C) Lawrence Lo (https://github.com/galliumstudio). 
 * All rights reserved.
 *
 * This program is open source software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include "stm32f4xx_nucleo.h"
#include "hsm_id.h"
#include "fw_log.h"
#include "UserLed.h"
#include "event.h"
#include "LedPattern.h"

Q_DEFINE_THIS_FILE

namespace APP {

#define  PERIOD_VALUE       (uint32_t)(1000 - 1)  /* Period Value  */
#define  PULSE1_VALUE       (uint32_t)((PERIOD_VALUE + 1)/2) /* Capture Compare 1 Value  */
  
bool UserLed::ConfigPwm() {  
    TIM_OC_InitTypeDef timConfig;
    /* Compute the prescaler value to have TIM3 counter clock equal to 21000000 Hz */
    uint32_t uhPrescalerValue = (uint32_t)((SystemCoreClock) / 21000000) - 1;

    /*##-1- Configure the TIM peripheral #######################################*/
    /* -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles.

      In this example TIM3 input clock (TIM3CLK) is set to APB1 clock x 2,
      since APB1 prescaler is equal to 2.
        TIM2CLK = APB1CLK*2
        APB1CLK = HCLK/2
        => TIM2CLK = HCLK = SystemCoreClock

      To get TIM3 counter clock at 16 MHz, the prescaler is computed as follows:
         Prescaler = (TIM2CLK / TIM3 counter clock) - 1
         Prescaler = ((SystemCoreClock) /15 MHz) - 1

      To get TIM3 output clock at 21 KHz, the period (ARR)) is computed as follows:
         ARR = (TIM3 counter clock / TIM3 output clock) - 1
             = 665

      TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR + 1)* 100 = 50%
      TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR + 1)* 100 = 37.5%
      TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR + 1)* 100 = 25%
      TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR + 1)* 100 = 12.5%

      Note:
       SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
       Each time the core clock (HCLK) changes, user had to update SystemCoreClock
       variable value. Otherwise, any configuration based on this variable will be incorrect.
       This variable is updated in three ways:
        1) by calling CMSIS function SystemCoreClockUpdate()
        2) by calling HAL API function HAL_RCC_GetSysClockFreq()
        3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
    ----------------------------------------------------------------------- */

    /* Initialize TIMx peripheral as follows:
         + Prescaler = ((SystemCoreClock) / 21000000) - 1
         + Period = (1000 - 1)
         + ClockDivision = 0
         + Counter direction = Up
    */
    m_timHandle.Instance = TIM2;

    m_timHandle.Init.Prescaler         = uhPrescalerValue;
    m_timHandle.Init.Period            = PERIOD_VALUE;
    m_timHandle.Init.ClockDivision     = 0;
    m_timHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    m_timHandle.Init.RepetitionCounter = 0;
    if (HAL_TIM_PWM_Init(&m_timHandle) != HAL_OK) {
        return false;
    }

    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    timConfig.OCMode       = TIM_OCMODE_PWM1;
    timConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    timConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    timConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    timConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    timConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

    /* Set the pulse value for channel 1 */
    timConfig.Pulse = PULSE1_VALUE;
    if (HAL_TIM_PWM_ConfigChannel(&m_timHandle, &timConfig, TIM_CHANNEL_1) != HAL_OK) {
        return false;
    }
    return true;
}
  
UserLed::UserLed() :
    QActive((QStateHandler)&UserLed::InitialPseudoState),
    m_id(USER_LED), m_name("USER_LED"), m_nextSequence(0), 
    m_stateTimer(this, USER_LED_STATE_TIMER) {}

QState UserLed::InitialPseudoState(UserLed * const me, QEvt const * const e) {
    (void)e;
    
    me->subscribe(USER_LED_START_REQ);
    me->subscribe(USER_LED_STOP_REQ);
    me->subscribe(USER_LED_ON_REQ);
    me->subscribe(USER_LED_OFF_REQ);  
    me->subscribe(USER_LED_STATE_TIMER);
    me->subscribe(USER_LED_DONE);
    
    return Q_TRAN(&UserLed::Root);
}

QState UserLed::Root(UserLed * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            status = Q_TRAN(&UserLed::Stopped);
            break;
        }
        case USER_LED_START_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedStartCfm(req.GetSeq(), ERROR_STATE);
            QF::PUBLISH(evt, me);
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(&QHsm::top);
            break;
        }
    }
    return status;
}

QState UserLed::Stopped(UserLed * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        case USER_LED_STOP_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedStopCfm(req.GetSeq(), ERROR_SUCCESS);
            QF::PUBLISH(evt, me);
            status = Q_HANDLED();
            break;
        }
        case USER_LED_START_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedStartCfm(req.GetSeq(), ERROR_SUCCESS);
            QF::PUBLISH(evt, me);
            status = Q_TRAN(&UserLed::Started);
            break;
        }
        default: {
            status = Q_SUPER(&UserLed::Root);
            break;
        }
    }
    return status;
}

QState UserLed::Started(UserLed * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOG_EVENT(e);       
            BSP_LED_Init(LED2);
            bool status = me->ConfigPwm();
            Q_ASSERT(status);
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOG_EVENT(e);
            BSP_LED_DeInit(LED2);
            status = Q_HANDLED();
            break;
        }
        case USER_LED_STOP_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedStopCfm(req.GetSeq(), ERROR_SUCCESS);
            QF::PUBLISH(evt, me);
            status = Q_TRAN(&UserLed::Stopped);
            break;
        }
        case USER_LED_ON_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedOnCfm(req.GetSeq(), ERROR_SUCCESS);
            QF::PUBLISH(evt, me);
            
            //BSP_LED_On(LED2);
            if (HAL_TIM_PWM_Start(&me->m_timHandle, TIM_CHANNEL_1) != HAL_OK) {
                Q_ASSERT(0);
            }
            
            // Test only.
            /*
            LedPatternSet const &p = TEST_LED_PATTERN_SET;
            PRINT("%p %d\n\r", &p, p.m_pattern[1].m_interval[0].m_durationMs);
            Q_ASSERT(0);
            */
            
            status = Q_HANDLED();
            break;
        }
        case USER_LED_OFF_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedOffCfm(req.GetSeq(), ERROR_SUCCESS);
            QF::PUBLISH(evt, me);   
            
            //BSP_LED_Off(LED2);
            if (HAL_TIM_PWM_Stop(&me->m_timHandle, TIM_CHANNEL_1) != HAL_OK) {
                Q_ASSERT(0);
            }
            
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(&UserLed::Root);
            break;
        }
    }
    return status;
}

/*
QState UserLed::MyState(UserLed * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            status = Q_TRAN(&UserLed::SubState);
            break;
        }
        default: {
            status = Q_SUPER(&UserLed::SuperState);
            break;
        }
    }
    return status;
}
*/

} // namespace APP
