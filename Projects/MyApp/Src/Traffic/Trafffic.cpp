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
#include "Traffic.h"
#include "event.h"

/*
#undef LOG_EVENT
#define LOG_EVENT(e)            
#undef DEBUG
#define DEBUG(x, y)
*/

Q_DEFINE_THIS_FILE

namespace APP {
  
void Traffic::PrintUsage() {
    // Print usage.
    PRINT("Type 'n' or 's' to indicate car arriving in north or south direction\n\r");
    PRINT("Type 'e' or 'w' to indicate car arriving in east or west direction\n\r");       
    PRINT("Type 'r' to indicate error condition\n\r");  
}
    
Traffic::Traffic() :
    QActive((QStateHandler)&Traffic::InitialPseudoState),
    m_id(TRAFFIC), m_name("TRAFFIC"), m_carWaiting(false),
    m_lampNS(LAMP_NS, "LAMP_NS", this), m_lampEW(LAMP_EW, "LAMP_EW", this),
    m_waitTimer(this, TRAFFIC_WAIT_TIMER), m_idleTimer(this, TRAFFIC_IDLE_TIMER),
    m_blinkTimer(this, TRAFFIC_BLINK_TIMER) {}

QState Traffic::InitialPseudoState(Traffic * const me, QEvt const * const e) {
    (void)e;
    me->subscribe(UART_IN_CHAR_IND);
    me->subscribe(TRAFFIC_START_REQ);
    me->subscribe(TRAFFIC_STOP_REQ);
    me->subscribe(TRAFFIC_CAR_NS_REQ);
    me->subscribe(TRAFFIC_CAR_EW_REQ);
    me->subscribe(TRAFFIC_WAIT_TIMER);
    me->subscribe(TRAFFIC_IDLE_TIMER);
    me->subscribe(TRAFFIC_BLINK_TIMER);
    me->subscribe(LAMP_RED_REQ);      
    me->subscribe(LAMP_YELLOW_REQ);   
    me->subscribe(LAMP_GREEN_REQ);
    me->subscribe(LAMP_OFF_REQ);    
    
    return Q_TRAN(&Traffic::Root);
}

QState Traffic::Root(Traffic * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            // Test only. Start 1000ms (1s) timer. Please remove.
            //me->m_waitTimer.armX(1000);
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOG_EVENT(e);
            // Test only. Stop timer. Please remove.
            //me->m_waitTimer.disarm();
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::Stopped);
            break;
        }
        case TRAFFIC_START_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new TrafficStartCfm(req.GetSeq(), ERROR_STATE);
            QF::PUBLISH(evt, me);
            status = Q_HANDLED();
            break;
        }  
        // Test only. Showing how to handle a timeout event. Please remove.
        /*
        case TRAFFIC_WAIT_TIMER: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        */
        case UART_IN_CHAR_IND: {
            UartInCharInd const &ind = static_cast<UartInCharInd const &>(*e);
            char ch = ind.GetChar();
            Evt *evt;
            switch(ch) {
                case 'n':
                case 's': {
                    evt = new Evt(TRAFFIC_CAR_NS_REQ);
                    break;
                }
                case 'e':
                case 'w': {
                    evt = new Evt(TRAFFIC_CAR_EW_REQ);
                    break;
                }
                case 'r': {
                    evt = new Evt(TRAFFIC_ERROR_REQ);
                    break;
                }                
                default:  evt = NULL; me->PrintUsage(); break;
            }
            if (evt) {
                me->postLIFO(evt);
            }
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

QState Traffic::Stopped(Traffic * const me, QEvt const * const e) {
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
        case TRAFFIC_STOP_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new TrafficStopCfm(req.GetSeq(), ERROR_SUCCESS);
            QF::PUBLISH(evt, me);
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_START_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new TrafficStartCfm(req.GetSeq(), ERROR_SUCCESS);
            QF::PUBLISH(evt, me);
            status = Q_TRAN(&Traffic::Started);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::Root);
            break;
        }
    }
    return status;
}

QState Traffic::Started(Traffic * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOG_EVENT(e);       
            status = Q_HANDLED();
            // Initialize regions.
            me->m_lampNS.Init();
            me->m_lampEW.Init();
            me->PrintUsage();
            break;
        }
        case Q_EXIT_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            status = Q_TRAN(&Traffic::NSGo);
            break;
        }
        case TRAFFIC_STOP_REQ: {
            LOG_EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new TrafficStopCfm(req.GetSeq(), ERROR_SUCCESS);
            QF::PUBLISH(evt, me);
            status = Q_TRAN(&Traffic::Stopped);
            break;
        }
        case TRAFFIC_ERROR_REQ: {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::StopSign);
            break;
        }
        
        // Test only - please remove.
        /*
        case TRAFFIC_CAR_NS_REQ: {
            LOG_EVENT(e);
            DEBUG("If you see this message, you have not implemented this event yet.");
            // Test posting to orthogonal region.
            Evt *evt = new LampRedReq(LAMP_NS);
            me->postLIFO(evt);
            status = Q_HANDLED();
            break;
        }
        // Test only - please remove.
        case TRAFFIC_CAR_EW_REQ: {
            LOG_EVENT(e);
            DEBUG("If you see this message, you have not implemented this event yet.");
            // Test posting to orthogonal region.
            Evt *evt = new LampGreenReq(LAMP_EW);
            me->postLIFO(evt);
            status = Q_HANDLED();
            break;
        }
        */
        
        // Dispatch events to orthogonal regions.
        case LAMP_RED_REQ:
        case LAMP_YELLOW_REQ:
        case LAMP_GREEN_REQ:
        case LAMP_OFF_REQ: {
            LampReq const &req = static_cast<LampReq const &>(*e);
            if (req.GetLampId() == LAMP_NS) {
                me->m_lampNS.dispatch(e);
            } else if (req.GetLampId() == LAMP_EW) {
                me->m_lampEW.dispatch(e);
            } else {
                Q_ASSERT(0);
            }
            status = Q_HANDLED();
            break;
        }    
        default: {
            status = Q_SUPER(&Traffic::Root);
            break;
        }
    }
    return status;
}

QState Traffic::NSGo(Traffic *me, QEvt const *e)
{
    QState status;
    switch (e->sig)
    {
        //----------------------------------------------------------------------
        case Q_ENTRY_SIG:
        {
            LOG_EVENT(e);
            Evt *evt = new LampGreenReq(LAMP_NS);
            me->postLIFO(evt);
            evt = new LampRedReq(LAMP_EW);
            me->postLIFO(evt);
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_EXIT_SIG:
        {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_INIT_SIG:
        {
            status = Q_TRAN(&Traffic::NSMinTimeWait);
            break;
        }
        case TRAFFIC_CAR_EW_REQ:
        {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::NSSlow);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::Started);
            break;
        }
    }
    return status;
}

QState Traffic::NSMinTimeWait(Traffic *me, QEvt const *e)
{
    QState status;    
    switch (e->sig)
    {
        //----------------------------------------------------------------------
        case Q_ENTRY_SIG:
        {
            LOG_EVENT(e);
            me->m_carWaiting = false;
            me->m_waitTimer.armX(NS_MIN_WAIT_TIMER_MS);
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_EXIT_SIG:
        {
            LOG_EVENT(e);
            if (me->m_carWaiting)
            {
                Evt *evt = new Evt(TRAFFIC_CAR_EW_REQ);
                QF::PUBLISH(evt, me);;
            }
            me->m_waitTimer.disarm();
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_CAR_EW_REQ:
        {
            LOG_EVENT(e);
            me->m_carWaiting = true;
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_WAIT_TIMER:
        {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::NSMinTimeExceeded);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::NSGo);
            break;
        }        
    }
    return status;
}

QState Traffic::NSMinTimeExceeded(Traffic *me, QEvt const *e)
{
    QState status;
    switch (e->sig)
    {
        //----------------------------------------------------------------------
        case Q_ENTRY_SIG:
        {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_EXIT_SIG:
        {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::NSGo);
            break;
        }        
    }
    return status;
}

QState Traffic::NSSlow(Traffic *me, QEvt const *e)
{
    QState status;
    switch (e->sig)
    {
        //----------------------------------------------------------------------
        case Q_ENTRY_SIG:
        {
            LOG_EVENT(e);
            Evt *evt = new LampYellowReq(LAMP_NS);
            me->postLIFO(evt);
            me->m_waitTimer.armX(NS_SLOW_TIMER_MS);
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_EXIT_SIG:
        {
            LOG_EVENT(e);
            me->m_waitTimer.disarm();
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_WAIT_TIMER:
        {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::EWGo);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::Started);
            break;
        }        
    }
    return status;
}

QState Traffic::EWGo(Traffic *me, QEvt const *e)
{
    QState status;
    switch (e->sig)
    {
        //----------------------------------------------------------------------
        case Q_ENTRY_SIG:
        {
            LOG_EVENT(e);
            Evt *evt = new LampGreenReq(LAMP_EW);
            me->postLIFO(evt);
            evt = new LampRedReq(LAMP_NS);
            me->postLIFO(evt);
            me->m_idleTimer.armX(EW_IDLE_TIMER_MS);
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_EXIT_SIG:
        {
            LOG_EVENT(e);
            me->m_idleTimer.disarm();
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_INIT_SIG:
        {
            status = Q_TRAN(&Traffic::EWMinTimeWait);
            break;
        }
        case TRAFFIC_CAR_EW_REQ:
        {
            LOG_EVENT(e);
            me->m_idleTimer.rearm(EW_IDLE_TIMER_MS);
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_CAR_NS_REQ:
        case TRAFFIC_IDLE_TIMER:
        {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::EWSlow);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::Started);
            break;
        }
    }
    return status;
}

QState Traffic::EWMinTimeWait(Traffic *me, QEvt const *e)
{
    QState status;
    switch (e->sig)
    {
        //----------------------------------------------------------------------
        case Q_ENTRY_SIG:
        {
            LOG_EVENT(e);
            me->m_carWaiting = false;
            me->m_waitTimer.armX(EW_MIN_WAIT_TIMER_MS);
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_EXIT_SIG:
        {
            LOG_EVENT(e);
            if (me->m_carWaiting)
            {
                Evt *evt = new Evt(TRAFFIC_CAR_NS_REQ);
                QF::PUBLISH(evt, me);;
            }
            me->m_waitTimer.disarm();
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_CAR_NS_REQ:
        {
            LOG_EVENT(e);
            me->m_carWaiting = true;
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_WAIT_TIMER:
        {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::EWMinTimeExceeded);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::EWGo);
            break;
        }        
    }
    return status;
}

QState Traffic::EWMinTimeExceeded(Traffic *me, QEvt const *e)
{
    QState status;
    switch (e->sig)
    {
        //----------------------------------------------------------------------
        case Q_ENTRY_SIG:
        {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_EXIT_SIG:
        {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::EWGo);
            break;
        }        
    }
    return status;
}

QState Traffic::EWSlow(Traffic *me, QEvt const *e)
{
    QState status;
    switch (e->sig)
    {
        //----------------------------------------------------------------------
        case Q_ENTRY_SIG:
        {
            LOG_EVENT(e);
            Evt *evt = new LampYellowReq(LAMP_EW);
            me->postLIFO(evt);
            me->m_waitTimer.armX(EW_SLOW_TIMER_MS);
            status = Q_HANDLED();
            break;
        }
        //----------------------------------------------------------------------
        case Q_EXIT_SIG:
        {
            LOG_EVENT(e);
            me->m_waitTimer.disarm();
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_WAIT_TIMER:
        {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::NSGo);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::Started);
            break;
        }        
    }
    return status;
}

QState Traffic::StopSign(Traffic * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOG_EVENT(e);
            me->m_blinkTimer.armX(BLINK_TIMER_MS, BLINK_TIMER_MS);               
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOG_EVENT(e);
            me->m_blinkTimer.disarm();
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::StopSignOn);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::Started);
            break;
        }
    }
    return status;
}

QState Traffic::StopSignOn(Traffic * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOG_EVENT(e);
            Evt *evt = new LampRedReq(LAMP_EW);
            me->postLIFO(evt);
            evt = new LampRedReq(LAMP_NS);
            me->postLIFO(evt);
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_BLINK_TIMER: {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::StopSignOff);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::StopSign);
            break;
        }
    }
    return status;
}

QState Traffic::StopSignOff(Traffic * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOG_EVENT(e);
            Evt *evt = new LampOffReq(LAMP_EW);
            me->postLIFO(evt);
            evt = new LampOffReq(LAMP_NS);
            me->postLIFO(evt);
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOG_EVENT(e);
            status = Q_HANDLED();
            break;
        }
        case TRAFFIC_BLINK_TIMER: {
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::StopSignOn);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::StopSign);
            break;
        }
    }
    return status;
}

/*
QState Traffic::MyState(Traffic * const me, QEvt const * const e) {
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
            LOG_EVENT(e);
            status = Q_TRAN(&Traffic::SubState);
            break;
        }
        default: {
            status = Q_SUPER(&Traffic::SuperState);
            break;
        }
    }
    return status;
}
*/

} // namespace APP
