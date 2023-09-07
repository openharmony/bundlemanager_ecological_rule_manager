/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "system_ability_definition.h"
#include "ecological_rule_mgr_service_client.h"
#include "ecological_rule_mgr_service.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
using namespace testing::ext;

class EcologicalRuleMgrServiceClientTest : public testing::Test {
public:
    EcologicalRuleMgrServiceClientTest() {}
    ~EcologicalRuleMgrServiceClientTest() {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    sptr<EcologicalRuleMgrServiceClient> ecologicalRuleMgrServiceClient;
};

void EcologicalRuleMgrServiceClientTest::SetUpTestCase(void) {}

void EcologicalRuleMgrServiceClientTest::TearDownTestCase(void) {}

void EcologicalRuleMgrServiceClientTest::SetUp()
{
    ecologicalRuleMgrServiceClient = OHOS::EcologicalRuleMgrService::EcologicalRuleMgrServiceClient::GetInstance();
}

void EcologicalRuleMgrServiceClientTest::TearDown()
{
    ecologicalRuleMgrServiceClient = nullptr;
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, QueryFreeInstallExperience_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "QueryFreeInstallExperience_Test1 start";
    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_HARMONY_APP;
    callerInfo.targetAppType = CallerInfo::TYPE_ATOM_SERVICE;
    ExperienceRule rule;

    int32_t res = ecologicalRuleMgrServiceClient->QueryFreeInstallExperience(want, callerInfo, rule);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ReturnCode::SUCCESS, res);
    GTEST_LOG_(INFO) << "QueryFreeInstallExperience_Test1 end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, QueryStartExperience_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "QueryStartExperience_Test1 start";
    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_HARMONY_APP;
    callerInfo.targetAppType = CallerInfo::TYPE_ATOM_SERVICE;
    ExperienceRule rule;

    int32_t res = ecologicalRuleMgrServiceClient->QueryStartExperience(want, callerInfo, rule);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ReturnCode::SUCCESS, res);
    GTEST_LOG_(INFO) << "QueryStartExperience_Test1 end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, IsSupportPublishForm_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsSupportPublishForm_Test start";
    Want want;
    want.SetBundle("com.myapplication.demo");
    std::vector<Want> wants;
    wants.push_back(want);
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    bool bSupport;

    int32_t res = ecologicalRuleMgrServiceClient->IsSupportPublishForm(wants, callerInfo, bSupport);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ReturnCode::SUCCESS, res);
    GTEST_LOG_(INFO) << "IsSupportPublishForm_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, EvaluateResolveInfos_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EvaluateResolveInfos_Test start";
    Want want;
    want.SetBundle("com.myapplication.demo");

    CallerInfo callerInfo;
    int32_t type = 1;

    vector<AbilityInfo> abilityInfos;
    AbilityInfo abilityInfo;
    abilityInfos.push_back(abilityInfo);

    vector<ExtensionAbilityInfo> extInfos;
    ExtensionAbilityInfo extInfo;
    extInfos.push_back(extInfo);

    int32_t res = ecologicalRuleMgrServiceClient->EvaluateResolveInfos(want, callerInfo, type, abilityInfos, extInfos);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ReturnCode::SUCCESS, res);
    GTEST_LOG_(INFO) << "EvaluateResolveInfos_Test end";
}
}
}