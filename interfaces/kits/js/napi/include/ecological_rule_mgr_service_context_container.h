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

#ifndef ECOLOGICALRULEMANAGERSERVICE_CONTEXT_CONTAINER_H
#define ECOLOGICALRULEMANAGERSERVICE_CONTEXT_CONTAINER_H

#include <unordered_map>
#include <mutex>

namespace OHOS {
namespace EcologicalRuleMgrServiceNapi {
class EcologicalRuleMgrServiceContextContainer {
public:
    EcologicalRuleMgrServiceContextContainer() = default;
    ~EcologicalRuleMgrServiceContextContainer();
    static EcologicalRuleMgrServiceContextContainer *GetInstance();
    void registerContext(uint32_t code, void *context);
    void *queryContext(uint32_t code);

private:
    static std::mutex instanceLock_;
    static EcologicalRuleMgrServiceContextContainer *instance_;
    std::unordered_map<uint32_t, void *> contextContainer_;
};
} // namespace EcologicalRuleMgrServiceNapi
} // namespace OHOS

#endif // ECOLOGICALRULEMANAGERSERVICE_CONTEXT_CONTAINER_H