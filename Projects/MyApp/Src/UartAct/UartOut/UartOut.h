/*******************************************************************************
 * Copyright (C) 2017 Gallium Studio LLC (Lawrence Lo). All rights reserved. 
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

#ifndef UART_OUT_H
#define UART_OUT_H

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "qpcpp.h"
#include "fw_evt.h"
#include "hsm_id.h"

using namespace QP;
using namespace FW;

namespace APP {

class UartOut : public QHsm {
public:
    UartOut(uint8_t id, char const *name, QActive *owner, UART_HandleTypeDef &hal);
    void Init() { 
      QHsm::init(); 
    }
    static void DmaCompleteCallback(uint8_t id);

protected:
    static QState InitialPseudoState(UartOut * const me, QEvt const * const e);
    static QState Root(UartOut * const me, QEvt const * const e);
        static QState Stopped(UartOut * const me, QEvt const * const e);
        static QState Started(UartOut * const me, QEvt const * const e);
            static QState Inactive(UartOut * const me, QEvt const * const e);
            static QState Active(UartOut * const me, QEvt const * const e);
                static QState Normal(UartOut * const me, QEvt const * const e);
                static QState StopWait(UartOut * const me, QEvt const * const e);
            static QState Failed(UartOut * const me, QEvt const * const e);

    enum {
        DEFER_QUEUE_COUNT = 4
    };
    QEvt const *m_deferQueueStor[DEFER_QUEUE_COUNT];
    QEQueue m_deferQueue;            
    uint8_t m_id;
    char const * m_name;
    uint16_t m_nextSequence;    
    QActive *m_owner;        
            
    UART_HandleTypeDef &m_hal;
    Fifo *m_fifo;
    uint32_t m_writeCount;
    QTimeEvt m_activeTimer;

    enum {
        ACTIVE_TIMEOUT_MS = 1000,
    };
};

} // namespace APP

#endif // UART_OUT_H
