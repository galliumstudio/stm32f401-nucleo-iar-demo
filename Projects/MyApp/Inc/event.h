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

#ifndef EVENT_H
#define EVENT_H

#include "qpcpp.h"
#include "fw_error.h"
#include "fw_evt.h"
#include "fw_pipe.h"

using namespace FW;

namespace APP {

enum {
    SYSTEM_START_REQ = QP::Q_USER_SIG,
    SYSTEM_START_CFM,
    SYSTEM_STOP_REQ,
    SYSTEM_STOP_CFM,
    SYSTEM_STATE_TIMER,
    SYSTEM_TEST_TIMER,
    SYSTEM_DONE,
    SYSTEM_FAIL,
    
    UART_ACT_START_REQ,
    UART_ACT_START_CFM,
    UART_ACT_STOP_REQ,
    UART_ACT_STOP_CFM,
    UART_ACT_FAIL_IND,
    UART_ACT_STATE_TIMER,
    UART_ACT_START,
    UART_ACT_DONE,
    UART_ACT_FAIL,
    
    UART_OUT_START_REQ,
    UART_OUT_START_CFM,
    UART_OUT_STOP_REQ,
    UART_OUT_STOP_CFM,
    UART_OUT_FAIL_IND,
    UART_OUT_WRITE_REQ,     // of type Evt
    UART_OUT_WRITE_CFM,     // of type ErrorEvt
    UART_OUT_EMPTY_IND,     // of type Evt
    UART_OUT_ACTIVE_TIMER,
    UART_OUT_DONE,
    UART_OUT_DMA_DONE,
    UART_OUT_CONTINUE,
    UART_OUT_HW_FAIL,
    
    UART_IN_START_REQ,
    UART_IN_START_CFM,
    UART_IN_STOP_REQ,
    UART_IN_STOP_CFM,
    UART_IN_CHAR_IND,   // For testing only (by-passing fifo)
    UART_IN_DATA_IND,   // of type Evt. Data received and stored in fifo.
    UART_IN_FAIL_IND,
    UART_IN_ACTIVE_TIMER,
    UART_IN_DONE,
    UART_IN_DATA_RDY,   // of type Evt
    UART_IN_DMA_RECV,   // of type Evt
    UART_IN_OVERFLOW,   // of type Evt
    UART_IN_HW_FAIL,   // of type Evt
    
    USER_BTN_START_REQ,
    USER_BTN_START_CFM,
    USER_BTN_STOP_REQ,
    USER_BTN_STOP_CFM,
    USER_BTN_UP_IND,    // of type Evt
    USER_BTN_DOWN_IND,  // of type Evt
    USER_BTN_HOLD_IND,  // of type Evt
    USER_BTN_STATE_TIMER,
    USER_BTN_HOLD_TIMER,
    USER_BTN_TRIG,
    USER_BTN_UP,
    USER_BTN_DOWN,
    
    USER_LED_START_REQ,
    USER_LED_START_CFM,
    USER_LED_STOP_REQ,
    USER_LED_STOP_CFM,
    USER_LED_ON_REQ,
    USER_LED_ON_CFM,
    USER_LED_OFF_REQ,
    USER_LED_OFF_CFM,   
    USER_LED_STATE_TIMER,
    USER_LED_DONE,
    
    MAX_PUB_SIG
};

char const * GetEvtName(QP::QSignal sig);

class SystemStartReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 600
    };
    SystemStartReq(uint16_t seq) :
        Evt(SYSTEM_START_REQ, seq) {}
};

class SystemStartCfm : public ErrorEvt {
public:
    SystemStartCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(SYSTEM_START_CFM, seq, error, reason) {}
};

class SystemStopReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 600
    };
    SystemStopReq(uint16_t seq) :
        Evt(SYSTEM_STOP_REQ, seq) {}
};

class SystemStopCfm : public ErrorEvt {
public:
    SystemStopCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(SYSTEM_STOP_CFM, seq, error, reason) {}
};

class SystemFail : public ErrorEvt {
public:
    SystemFail(Error error, Reason reason) :
        ErrorEvt(SYSTEM_FAIL, 0, error, reason) {}
};

class UartActStartReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 200
    };
    UartActStartReq(uint16_t seq, Fifo *outFifo, Fifo *inFifo) :
        Evt(UART_ACT_START_REQ, seq), m_outFifo(outFifo), m_inFifo(inFifo) {}
    Fifo *GetOutFifo() const { return m_outFifo; }
    Fifo *GetInFifo() const { return m_inFifo; }
private:
    Fifo *m_outFifo;
    Fifo *m_inFifo;
};

class UartActStartCfm : public ErrorEvt {
public:
    UartActStartCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(UART_ACT_START_CFM, seq, error, reason) {}
};

class UartActStopReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 200
    };
    UartActStopReq(uint16_t seq) :
        Evt(UART_ACT_STOP_REQ, seq) {}
};

class UartActStopCfm : public ErrorEvt {
public:
    UartActStopCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(UART_ACT_STOP_CFM, seq, error, reason) {}
};

class UartActFailInd : public ErrorEvt {
public:
    UartActFailInd(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(UART_ACT_FAIL_IND, seq, error, reason) {}
};

class UartActFail : public ErrorEvt {
public:
    UartActFail(Error error, Reason reason) :
        ErrorEvt(UART_ACT_FAIL, 0, error, reason) {}
};

class UartOutStartReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UartOutStartReq(uint16_t seq, Fifo *fifo) :
        Evt(UART_OUT_START_REQ, seq), m_fifo(fifo) {}
    Fifo *GetFifo() const { return m_fifo; }
private:
    Fifo *m_fifo;
};

class UartOutStartCfm : public ErrorEvt {
public:
    UartOutStartCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(UART_OUT_START_CFM, seq, error, reason) {}
};

class UartOutStopReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UartOutStopReq(uint16_t seq) :
        Evt(UART_OUT_STOP_REQ, seq) {}
};

class UartOutStopCfm : public ErrorEvt {
public:
    UartOutStopCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(UART_OUT_STOP_CFM, seq, error, reason) {}
};

class UartOutFailInd : public ErrorEvt {
public:
    UartOutFailInd(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(UART_OUT_FAIL_IND, seq, error, reason) {}
};

class UartInStartReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UartInStartReq(uint16_t seq, Fifo *fifo) :
        Evt(UART_IN_START_REQ, seq), m_fifo(fifo) {}
    Fifo *GetFifo() const { return m_fifo; }
private:
    Fifo *m_fifo;
};

class UartInStartCfm : public ErrorEvt {
public:
    UartInStartCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(UART_IN_START_CFM, seq, error, reason) {}
};

class UartInStopReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UartInStopReq(uint16_t seq) :
        Evt(UART_IN_STOP_REQ, seq) {}
};

class UartInStopCfm : public ErrorEvt {
public:
    UartInStopCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(UART_IN_STOP_CFM, seq, error, reason) {}
};

class UartInCharInd : public Evt {
public:
    UartInCharInd(uint16_t seq, char ch) :
        Evt(UART_IN_CHAR_IND, seq), m_ch(ch) {}
    char GetChar() const { return m_ch; }
private:
    char m_ch;
};

class UartInFailInd : public ErrorEvt {
public:
    UartInFailInd(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(UART_IN_FAIL_IND, seq, error, reason) {}
};

class UserBtnStartReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserBtnStartReq(uint16_t seq) :
        Evt(USER_BTN_START_REQ, seq) {}
};

class UserBtnStartCfm : public ErrorEvt {
public:
    UserBtnStartCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(USER_BTN_START_CFM, seq, error, reason) {}
};

class UserBtnStopReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserBtnStopReq(uint16_t seq) :
        Evt(USER_BTN_STOP_REQ, seq) {}
};

class UserBtnStopCfm : public ErrorEvt {
public:
    UserBtnStopCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(USER_BTN_STOP_CFM, seq, error, reason) {}
};

class UserLedStartReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserLedStartReq(uint16_t seq) :
        Evt(USER_LED_START_REQ, seq) {}
};

class UserLedStartCfm : public ErrorEvt {
public:
    UserLedStartCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(USER_LED_START_CFM, seq, error, reason) {}
};

class UserLedStopReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserLedStopReq(uint16_t seq) :
        Evt(USER_LED_STOP_REQ, seq) {}
};

class UserLedStopCfm : public ErrorEvt {
public:
    UserLedStopCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(USER_LED_STOP_CFM, seq, error, reason) {}
};

class UserLedOnReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserLedOnReq(uint16_t seq) :
        Evt(USER_LED_ON_REQ, seq) {}
};

class UserLedOnCfm : public ErrorEvt {
public:
    UserLedOnCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(USER_LED_ON_CFM, seq, error, reason) {}
};

class UserLedOffReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserLedOffReq(uint16_t seq) :
        Evt(USER_LED_OFF_REQ, seq) {}
};

class UserLedOffCfm : public ErrorEvt {
public:
    UserLedOffCfm(uint16_t seq, Error error, Reason reason = 0) :
        ErrorEvt(USER_LED_OFF_CFM, seq, error, reason) {}
};

}

#endif