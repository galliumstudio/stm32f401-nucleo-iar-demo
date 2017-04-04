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

#ifndef TEST_H
#define TEST_H

#include "bsp.h"
#include <stdint.h>
#include "fw_log.h"
#include "qassert.h"

//#define TEST_ASSERT(t_) ((t_)? (void)0: Q_onAssert("Test.h", (int_t)__LINE__))

using namespace FW;

namespace APP {

void TestFunction();
  
// Before C++...
typedef struct {
    uint32_t a;
    uint32_t b;
} TestStruct;

uint32_t TestSum(TestStruct *ts);
uint32_t TestSub(TestStruct *ts);
  
// With C++ Encapsulation
class TestClass {
private:
    uint32_t m_a;
    uint32_t m_b;    
public:
  TestClass(uint32_t a, uint32_t b) : m_a(a), m_b(b) {}
  uint32_t GetA() const { return m_a; }
  uint32_t GetB() const { return m_b; }
  uint32_t Sum() const { return m_a + m_b; }
};
  

// With C++ Inheritance and Polymorphism.
class TestBase {
public:
  TestBase(uint32_t id = 0) : m_id(id) {}
  virtual void Print() {
    PRINT("TestBase m_id = %d\n\r", m_id);
  }
protected:
  uint32_t m_id;
};

class TestDerived1 : public TestBase {
public:
  TestDerived1(uint32_t id = 127) : TestBase(id) {}
  void Print() {
    PRINT("TestDerived1 m_id = 0x%x\n\r", m_id);
  }
};


}

#endif //