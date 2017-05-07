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

#include "qpcpp.h"
#include "event.h"
#include "fw_macro.h"

Q_DEFINE_THIS_FILE

namespace APP {
  
char const * const eventName[] = {
    "NULL",
    "ENTRY",
    "EXIT",
    "INIT",
    "SYSTEM_START_REQ",
    "SYSTEM_START_CFM",
    "SYSTEM_STOP_REQ",
    "SYSTEM_STOP_CFM",
    "SYSTEM_STATE_TIMER",
    "SYSTEM_TEST_TIMER",
    "SYSTEM_DONE",
    "SYSTEM_FAIL",
    
    "UART_ACT_START_REQ",
    "UART_ACT_START_CFM",
    "UART_ACT_STOP_REQ",
    "UART_ACT_STOP_CFM",
    "UART_ACT_FAIL_IND",
    "UART_ACT_STATE_TIMER",
    "UART_ACT_START",
    "UART_ACT_DONE",
    "UART_ACT_FAIL",
      
    "UART_OUT_START_REQ",
    "UART_OUT_START_CFM",
    "UART_OUT_STOP_REQ",
    "UART_OUT_STOP_CFM",
    "UART_OUT_FAIL_IND",
    "UART_OUT_WRITE_REQ", 
    "UART_OUT_WRITE_CFM", 
    "UART_OUT_EMPTY_IND",
    "UART_OUT_ACTIVE_TIMER",
    "UART_OUT_DONE",
    "UART_OUT_DMA_DONE",
    "UART_OUT_CONTINUE",
    "UART_OUT_HW_FAIL",
    
    "UART_IN_START_REQ",
    "UART_IN_START_CFM",
    "UART_IN_STOP_REQ",
    "UART_IN_STOP_CFM",
    "UART_IN_CHAR_IND",
    "UART_IN_DATA_IND",
    "UART_IN_FAIL_IND",
    "UART_IN_ACTIVE_TIMER",
    "UART_IN_DONE",
    "UART_IN_DATA_RDY",
    "UART_IN_DMA_RECV",
    "UART_IN_OVERFLOW",
    "UART_IN_HW_FAIL",
    
    "USER_BTN_START_REQ",
    "USER_BTN_START_CFM",
    "USER_BTN_STOP_REQ",
    "USER_BTN_STOP_CFM",
    "USER_BTN_UP_IND",
    "USER_BTN_DOWN_IND",
    "USER_BTN_HOLD_IND",
    "USER_BTN_STATE_TIMER",
    "USER_BTN_HOLD_TIMER",
    "USER_BTN_TRIG",
    "USER_BTN_UP",
    "USER_BTN_DOWN",
    
    "USER_LED_START_REQ",
    "USER_LED_START_CFM",
    "USER_LED_STOP_REQ",
    "USER_LED_STOP_CFM",
    "USER_LED_ON_REQ",
    "USER_LED_ON_CFM",
    "USER_LED_OFF_REQ",
    "USER_LED_OFF_CFM", 
    "USER_LED_STATE_TIMER",
    "USER_LED_DONE",
};  
  
char const * GetEvtName(QP::QSignal sig) {
    Q_ASSERT(sig < ARRAY_COUNT(eventName));
    if (sig < MAX_PUB_SIG) {
      return eventName[sig];
    }
    return "(UNKNOWN)";
}

}