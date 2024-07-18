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

#ifndef ECOLOGICALRULEMANAGERSERVICE_PARAM_H
#define ECOLOGICALRULEMANAGERSERVICE_PARAM_H

#include <string>
#include <vector>
#include "ability_info.h"
#include "parcel.h"
#include "want.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
using Want = OHOS::AAFwk::Want;

struct ExperienceRule : public Parcelable {
    bool isAllow = true;
    int32_t resultCode = -1;
    sptr<Want> replaceWant = nullptr;
    bool Marshalling(Parcel &parcel) const override;
    static ExperienceRule *Unmarshalling(Parcel &parcel);
};

struct AmsExperienceRule : public Parcelable {
    int32_t resultCode = -1;
    sptr<Want> replaceWant = nullptr;
    bool Marshalling(Parcel &parcel) const override;
    static AmsExperienceRule *Unmarshalling(Parcel &parcel);
};

struct BmsExperienceRule : public Parcelable {
    bool isAllow = true;
    sptr<Want> replaceWant = nullptr;
    bool Marshalling(Parcel &parcel) const override;
    static BmsExperienceRule *Unmarshalling(Parcel &parcel);
};

struct CallerInfo : public Parcelable {
    enum {
        TYPE_INVALID = 0,
        TYPE_HARMONY_APP,
        TYPE_ATOM_SERVICE
    };
    enum {
        LINK_TYPE_INVALID = 0,
        LINK_TYPE_UNIVERSAL_LINK,
        LINK_TYPE_DEEP_LINK,
        LINK_TYPE_WEB_LINK,
        LINK_TYPE_ABILITY
    };
    std::string packageName;
    int32_t uid = 0L;
    int32_t pid = 0L;
    int32_t callerAppType = TYPE_INVALID;
    int32_t targetAppType = TYPE_INVALID;
    int32_t callerModeType = 0L;
    std::string targetAppDistType = "";
    std::string targetLinkFeature = "";
    int32_t targetLinkType = LINK_TYPE_INVALID;
    AppExecFwk::AbilityType callerAbilityType = AppExecFwk::AbilityType::UNKNOWN;
    int32_t embedded = 0L;
    std::string callerAppProvisionType;
    std::string targetAppProvisionType;
    AppExecFwk::ExtensionAbilityType callerExtensionAbilityType = AppExecFwk::ExtensionAbilityType::UNSPECIFIED;
    AppExecFwk::AbilityType targetAbilityType = AppExecFwk::AbilityType::UNKNOWN;
    AppExecFwk::ExtensionAbilityType targetExtensionAbilityType = AppExecFwk::ExtensionAbilityType::UNSPECIFIED;
    bool ReadFromParcel(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
    static CallerInfo *Unmarshalling(Parcel &parcel);
    std::string ToString() const;
};
} // namespace EcologicalRuleMgrService
} // namespace OHOS
#endif // ECOLOGICALRULEMANAGERSERVICE_PARAM_H
