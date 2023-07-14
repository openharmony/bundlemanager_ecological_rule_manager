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

#include "ecological_rule_mgr_service_param.h"
#include "iremote_broker.h"
#include "iremote_object.h"

#include "ecological_rule_mgr_service_logger.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "ERMS_PARAM"

ExperienceRule *ExperienceRule::Unmarshalling(Parcel &in)
{
    auto *rule = new (std::nothrow) ExperienceRule();
    if (rule == nullptr) {
        return nullptr;
    }

    if (!in.ReadBool(rule->isAllow)) {
        delete rule;
        return nullptr;
    }

    if (!in.ReadString(rule->sceneCode)) {
        delete rule;
        return nullptr;
    }

    rule->replaceWant = in.ReadParcelable<Want>();

    return rule;
}

bool ExperienceRule::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteBool(isAllow)) {
        LOG_ERROR("write isAllow failed");
        return false;
    }

    if (!parcel.WriteString(sceneCode)) {
        LOG_ERROR("write sceneCode failed");
        return false;
    }
    if (!parcel.WriteParcelable(replaceWant)) {
        LOG_ERROR("write replaceWant failed");
        return false;
    }

    return true;
}

bool CallerInfo::ReadFromParcel(Parcel &parcel)
{
    LOG_INFO("read from parcel");
    return true;
}

CallerInfo *CallerInfo::Unmarshalling(Parcel &in)
{
    auto *info = new (std::nothrow) CallerInfo();
    if (info == nullptr) {
        LOG_ERROR("new callerInfo failed, return nullptr");
        return nullptr;
    }

    info->packageName = in.ReadString();
    LOG_INFO("read packageName result: %{public}s", info->packageName.c_str());

    if (!in.ReadInt32(info->uid)) {
        LOG_ERROR("read uid failed");
        delete info;
        return nullptr;
    }

    if (!in.ReadInt32(info->pid)) {
        LOG_ERROR("read pid failed");
        delete info;
        return nullptr;
    }

    if (!in.ReadInt32(info->callerAppType)) {
        LOG_ERROR("read callerAppType failed");
        delete info;
        return nullptr;
    }

    if (!in.ReadInt32(info->targetAppType)) {
        LOG_ERROR("read targetAppType failed");
        delete info;
        return nullptr;
    }

    return info;
}

bool CallerInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(packageName)) {
        LOG_ERROR("write packageName failed");
        return false;
    }

    if (!parcel.WriteInt32(uid)) {
        LOG_ERROR("write uid failed");
        return false;
    }

    if (!parcel.WriteInt32(pid)) {
        LOG_ERROR("write pid failed");
        return false;
    }

    if (!parcel.WriteInt32(callerAppType)) {
        LOG_ERROR("write callerAppType failed");
        return false;
    }

    if (!parcel.WriteInt32(targetAppType)) {
        LOG_ERROR("write targetAppType failed");
        return false;
    }
    return true;
}

std::string CallerInfo::ToString() const
{
    std::string str = "CallerInfo{packageName:" + packageName + ",uid:" + std::to_string(uid) +
        ",pid:" + std::to_string(pid) + ",callerAppType:" + std::to_string(callerAppType) +
        ",targetAppType:" + std::to_string(targetAppType) + "}";
    return str;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS
