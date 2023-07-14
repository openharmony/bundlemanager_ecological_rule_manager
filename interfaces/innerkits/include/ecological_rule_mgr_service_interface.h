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

#ifndef SERVICES_INCLUDE_ECOLOGICALRULEMANAGERSERVICE_INTERFACE_H
#define SERVICES_INCLUDE_ECOLOGICALRULEMANAGERSERVICE_INTERFACE_H

#include <string>
#include "iremote_broker.h"
#include "ecological_rule_mgr_service_param.h"
#include "want.h"
#include "ability_info.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
class EcologicalRuleMgrServiceInterface : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.cloud.ecologicalrulemgrservice.EcologicalRuleMgrServiceInterface");

    using Want = OHOS::AAFwk::Want;

    using AbilityInfo = OHOS::AppExecFwk::AbilityInfo;

    virtual int32_t QueryFreeInstallExperience(const Want &want, const CallerInfo &callerInfo,
        ExperienceRule &rule) = 0;
    virtual int32_t QueryStartExperience(const Want &want, const CallerInfo &callerInfo, ExperienceRule &rule) = 0;
    virtual int32_t EvaluateResolveInfos(const Want &want, const CallerInfo &callerInfo, int32_t type,
        std::vector<AbilityInfo> &abilityInfos) = 0;
    virtual int32_t IsSupportPublishForm(const std::vector<Want> &wants, const CallerInfo &callerInfo,
        bool &bSupport) = 0;

    virtual int32_t SetAppEnhancedData(const int32_t &operType, const std::string &appData) = 0;
    virtual int32_t SetRuleInfo(const std::string &ruleInfo) = 0;
    virtual int32_t ExemptExperience(const std::string &targetBundleName, const int32_t &rule,
        const int32_t &timestamp) = 0;
    virtual int32_t GetVersion(std::string &versionJson) = 0;
    virtual int32_t SetSceneExperience(std::string &ruleConfig, std::string &sceneExperience) = 0;

    virtual int32_t GetSceneCode(const std::string &bundleName, std::string &sceneCode) = 0;
    virtual int32_t GetInitialSceneCode(const std::string &bundleName, std::string &originalSceneCode) = 0;

    virtual int32_t GetSelfSceneCode(std::string &sceneCode) = 0;
    virtual int32_t GetAdsVerificationVersion(int32_t &version) = 0;

    enum {
        QUERY_FREE_INSTALL_EXPERIENCE_CMD = 0,
        QUERY_START_EXPERIENCE_CMD,
        EVALUATE_RESOLVE_INFO_CMD,
        IS_SUPPORT_PUBLISH_FORM_CMD,
        GET_ENGINE_VERSION_CMD = 10,
        SET_EXEMPT_EXPERIENCE_CMD,
        SET_RULE_INFO_CMD,
        SET_APP_ENHANCED_DATA_CMD,
        SET_SCENE_EXPERIENCE_CMD,
        GET_SCENE_CODE_CMD = 20,
        GET_ORIGINAL_SCENE_CODE_CMD,
        GET_SELF_SCENE_CODE_CMD = 30,
        GET_ADS_VERIFICATION_VERSION_CMD
    };

    enum ErrCode {
        ERR_BASE = (-99),
        ERR_FAILED = (-1),
        ERR_PERMISSION_DENIED = (-2),
        ERR_OK = 0,
    };
};
} // namespace EcologicalRuleMgrService
} // namespace OHOS

#endif // SERVICES_INCLUDE_ECOLOGICALRULEMGRSERVICE_INTERFACE_H