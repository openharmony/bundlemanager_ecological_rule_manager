/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ecological_rule_mgr_service_context_container.h"

namespace OHOS {
namespace EcologicalRuleMgrServiceNapi {
std::mutex EcologicalRuleMgrServiceContextContainer::instanceLock_;
EcologicalRuleMgrServiceContextContainer *EcologicalRuleMgrServiceContextContainer::instance_;

EcologicalRuleMgrServiceContextContainer::~EcologicalRuleMgrServiceContextContainer()
{
    contextContainer_.clear();
}

EcologicalRuleMgrServiceContextContainer *EcologicalRuleMgrServiceContextContainer::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new EcologicalRuleMgrServiceContextContainer();
        }
    }
    return instance_;
}

void EcologicalRuleMgrServiceContextContainer::registerContext(uint32_t code, void *context)
{
    if (contextContainer_.find(code) == contextContainer_.end()) {
        contextContainer_[code] = context;
    }
}

void *EcologicalRuleMgrServiceContextContainer::queryContext(uint32_t code)
{
    if (contextContainer_.find(code) != contextContainer_.end()) {
        return contextContainer_[code];
    }
    return nullptr;
}
} // namespace EcologicalRuleMgrServiceNapi
} // namespace OHOS