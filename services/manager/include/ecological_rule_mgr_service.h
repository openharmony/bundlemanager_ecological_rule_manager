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

#ifndef SERVICES_INCLUDE_ECOLOGICALRULEMGRSERVICE_H
#define SERVICES_INCLUDE_ECOLOGICALRULEMGRSERVICE_H

#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>
#include "system_ability.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "ecological_rule_mgr_service_stub.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
using namespace OHOS::AppExecFwk;
using namespace std;

enum class ServiceRunningState {
    STATE_NOT_START,
    STATE_RUNNING
};
enum ReturnCode {
    SUCCESS = 0,
    ERROR = -1,
};

class EcologicalRuleMgrService : public SystemAbility, public EcologicalRuleMgrServiceStub {
    DECLARE_SYSTEM_ABILITY(EcologicalRuleMgrService);

public:
    using Want = OHOS::AAFwk::Want;

    DISALLOW_COPY_AND_MOVE(EcologicalRuleMgrService);
    EcologicalRuleMgrService();
    EcologicalRuleMgrService(int32_t saId, bool runOnCreate);
    ~EcologicalRuleMgrService();
    static sptr<EcologicalRuleMgrService> GetInstance();
    int32_t QueryFreeInstallExperience(const Want &want, const CallerInfo &callerInfo, ExperienceRule &rule) override;
    int32_t EvaluateResolveInfos(const Want &want, const CallerInfo &callerInfo, int32_t type,
        vector<AbilityInfo> &abilityInfos) override;
    int32_t QueryStartExperience(const Want &want, const CallerInfo &callerInfo, ExperienceRule &rule) override;
    int32_t IsSupportPublishForm(const vector<Want> &wants, const CallerInfo &callerInfo, bool &bSupport) override;

protected:
    void OnStart() override;
    void OnStop() override;

private:
    static mutex instanceLock_;
    static sptr<EcologicalRuleMgrService> instance_;
    ServiceRunningState state_;
};
} // EcologicalRuleMgrService
} // namespace OHOS

#endif // SERVICES_INCLUDE_ECOLOGICALRULEMGRSERVICE_H