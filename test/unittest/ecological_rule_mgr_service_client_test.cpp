/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include "base_exp.h"
#include "ecological_rule_engine_constants.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
using namespace testing::ext;

const std::string PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST = "ohos.permission.MANAGE_ECOLOGICAL_RULE";
const std::string PERMISSION_GET_SCENE_CODE_TEST = "ohos.permission.GET_SCENE_CODE";

void NativeTokenGet(const char *perms[], int size)
{
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = size,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_basic",
    };

    infoInstance.processName = "EcologicalRuleMgrUnitTest";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

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

void EcologicalRuleMgrServiceClientTest::SetUpTestCase(void)
{
    std::string ruleInfo =
        "{\"dataVersion\":\"1\",\"list\":[{\"condition\":\"OR(EQUAL(BUNDLENAME(want),\\\"com.myapplication.test\\\"),"
        "IS_JUMP_TYPE(caller,want,\\\"2\\\"))\",\"experience\":{\"load\":2,\"open\":2,\"out\":1},\"sceneCode\":"
        "\"10000\"},{\"condition\":\"AND(IS_JUMP_TYPE(caller,want,\\\"2\\\"),EQUAL(BUNDLENAME(caller),\\\"com."
        "myapplication.test\\\"))\",\"experience\":{\"load\":2,\"open\":2,\"out\":1},\"sceneCode\":\"10001\"}]}";
    std::string ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    std::string sceneExperience =
        "{\"sceneExperience\":{\"1\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"deviceId\\\":"
        "\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\","
        "\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.LOAD_ABILITY\\\",\\\"entities\\\":null,"
        "\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{\\\\\\\"5\\\\\\\":"
        "\\\\\\\"1002\\\\\\\"}}\\\"}\"}},\"2\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{"
        "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\","
        "\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"2002\\\\\\\"}}\\\"}\"}},\"5\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,"
        "\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":"
        "\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"5002\\\\\\\"}}\\\"}\"}}}}";
    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);
    OHOS::EcologicalRuleMgrService::EcologicalRuleMgrServiceClient::GetInstance()->SetRuleInfo(ruleInfo);
    OHOS::EcologicalRuleMgrService::EcologicalRuleMgrServiceClient::GetInstance()->SetSceneExperience(ruleConfig,
        sceneExperience);
}

void EcologicalRuleMgrServiceClientTest::TearDownTestCase(void) {}

void EcologicalRuleMgrServiceClientTest::SetUp()
{
    ecologicalRuleMgrServiceClient = OHOS::EcologicalRuleMgrService::EcologicalRuleMgrServiceClient::GetInstance();
}

void EcologicalRuleMgrServiceClientTest::TearDown()
{
    ecologicalRuleMgrServiceClient = nullptr;
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, QueryFreeInstallExperience_Test1, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "QueryFreeInstallExperience_Test1 start";
    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_HARMONY_APP;
    callerInfo.targetAppType = CallerInfo::TYPE_ATOM_SERVICE;
    ExperienceRule rule;

    // 加载体验: 弹框确认后加载
    int32_t res = ecologicalRuleMgrServiceClient->QueryFreeInstallExperience(want, callerInfo, rule);
    ASSERT_FALSE(rule.isAllow);
    EXPECT_STREQ("10000", rule.sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // caller包名为空，直接返回允许
    callerInfo.packageName = "";
    res = ecologicalRuleMgrServiceClient->QueryFreeInstallExperience(want, callerInfo, rule);
    ASSERT_TRUE(rule.isAllow);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "QueryFreeInstallExperience_Test1 end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, QueryFreeInstallExperience_Test2, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "QueryFreeInstallExperience_Test2 start";
    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_HARMONY_APP;
    callerInfo.targetAppType = CallerInfo::TYPE_ATOM_SERVICE;
    ExperienceRule rule;

    // add permission
    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    // sceneExperience data error, missing json key: "sceneExperience"
    std::string ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    std::string sceneExperience =
        "{\"experience\":{\"1\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"deviceId\\\":\\\"\\\","
        "\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\","
        "\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.LOAD_ABILITY\\\",\\\"entities\\\":null,"
        "\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{\\\\\\\"5\\\\\\\":"
        "\\\\\\\"1002\\\\\\\"}}\\\"}\"}},\"2\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{"
        "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\","
        "\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"2002\\\\\\\"}}\\\"}\"}},\"5\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,"
        "\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":"
        "\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"5002\\\\\\\"}}\\\"}\"}}}}";
    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);

    int32_t res = ecologicalRuleMgrServiceClient->QueryFreeInstallExperience(want, callerInfo, rule);
    ASSERT_TRUE(rule.isAllow);
    EXPECT_STREQ("00000", rule.sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // sceneExperience data error, error experienceValue
    ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    sceneExperience = "{\"sceneExperience\":{\"3\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{"
                      "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":"
                      "\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience."
                      "LOAD_ABILITY\\\",\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_"
                      "open_type\\\\\\\":{\\\\\\\"5\\\\\\\":\\\\\\\"1002\\\\\\\"}}\\\"}\"}}}}";
    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);
    res = ecologicalRuleMgrServiceClient->QueryFreeInstallExperience(want, callerInfo, rule);
    ASSERT_TRUE(rule.isAllow);
    EXPECT_STREQ("00000", rule.sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // sceneExperience data error, missing json key: "allow"
    ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    sceneExperience =
        "{\"sceneExperience\":{\"1\":{\"1\":{},\"2\":{\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":"
        "\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,"
        "\\\"action\\\":\\\"com.ecological.experience.LOAD_ABILITY\\\",\\\"entities\\\":null,\\\"parameters\\\":\\\"{"
        "\\\\\\\"ecological_experience_open_type\\\\\\\":{\\\\\\\"5\\\\\\\":\\\\\\\"1002\\\\\\\"}}\\\"}\"}},\"2\":{"
        "\"1\":{},\"2\":{\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":"
        "\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological."
        "experience.OPEN_ABILITY\\\",\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_"
        "type\\\\\\\":{\\\\\\\"5\\\\\\\":\\\\\\\"2002\\\\\\\"}}\\\"}\"}},\"5\":{\"1\":{},\"2\":{\"want\":\"{"
        "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\","
        "\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"5002\\\\\\\"}}\\\"}\"}}}}";
    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);
    res = ecologicalRuleMgrServiceClient->QueryFreeInstallExperience(want, callerInfo, rule);
    ASSERT_TRUE(rule.isAllow);
    EXPECT_STREQ("00000", rule.sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // sceneExperience data error, error want
    ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    sceneExperience =
        "{\"sceneExperience\":{\"1\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"Name\\\":\\\"\\\"}"
        "\"}},\"2\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"Name\\\":\\\"\\\"}\"}},\"5\":{\"1\":"
        "{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"Name\\\":\\\"\\\"}\"}}}}";
    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);
    callerInfo.packageName = "com.myapplication.test1";
    res = ecologicalRuleMgrServiceClient->QueryFreeInstallExperience(want, callerInfo, rule);
    ASSERT_FALSE(rule.isAllow);
    EXPECT_STREQ("10000", rule.sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "QueryFreeInstallExperience_Test2 end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, QueryStartExperience_Test1, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "QueryStartExperience_Test1 start";
    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_HARMONY_APP;
    callerInfo.targetAppType = CallerInfo::TYPE_ATOM_SERVICE;
    ExperienceRule rule;

    std::string ruleInfo =
        "{\"dataVersion\":\"1\",\"list\":[{\"condition\":\"OR(EQUAL(BUNDLENAME(want),\\\"com.myapplication.test\\\"),"
        "IS_JUMP_TYPE(caller,want,\\\"1\\\"))\",\"experience\":{\"load\":2,\"open\":2},\"sceneCode\":\"10000\"},{"
        "\"condition\":\"AND(IS_JUMP_TYPE(caller,want,\\\"2\\\"),EQUAL(BUNDLENAME(caller),\\\"com.myapplication."
        "test\\\"))\",\"experience\":{\"load\":2,\"open\":2},\"sceneCode\":\"10001\"}]}";
    std::string ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    std::string sceneExperience =
        "{\"sceneExperience\":{\"1\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"deviceId\\\":"
        "\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\","
        "\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.LOAD_ABILITY\\\",\\\"entities\\\":null,"
        "\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{\\\\\\\"5\\\\\\\":"
        "\\\\\\\"1002\\\\\\\"}}\\\"}\"}},\"2\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{"
        "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\","
        "\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"2002\\\\\\\"}}\\\"}\"}},\"5\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,"
        "\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":"
        "\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"5002\\\\\\\"}}\\\"}\"}}}}";
    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);
    ecologicalRuleMgrServiceClient->SetRuleInfo(ruleInfo);
    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);

    // 拉起体验: 弹框提示后拉起
    int32_t res = ecologicalRuleMgrServiceClient->QueryStartExperience(want, callerInfo, rule);
    ASSERT_FALSE(rule.isAllow);
    EXPECT_STREQ("10001", rule.sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // callerInfo包名为空，直接放通
    callerInfo.packageName = "";
    res = ecologicalRuleMgrServiceClient->QueryStartExperience(want, callerInfo, rule);
    ASSERT_TRUE(rule.isAllow);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "QueryStartExperience_Test1 end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, QueryStartExperience_Test2, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "QueryStartExperience_Test1 start";
    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_HARMONY_APP;
    callerInfo.targetAppType = CallerInfo::TYPE_ATOM_SERVICE;
    ExperienceRule rule;

    // 拉起体验: 直接拉起
    std::string ruleInfo =
        "{\"dataVersion\":\"1\",\"list\":[{\"condition\":\"OR(IS_JUMP_TYPE(caller,want,\\\"1\\\"),EQUAL(BUNDLENAME("
        "want),\\\"com.myapplication.test\\\"))\",\"experience\":{\"load\":1,\"open\":1,\"out\":1},\"sceneCode\":"
        "\"10000\"},{\"condition\":\"AND(IS_JUMP_TYPE(caller,want,\\\"2\\\"),EQUAL(BUNDLENAME(caller),\\\"com."
        "myapplication.test\\\"))\",\"experience\":{\"load\":1,\"open\":1,\"out\":1},\"sceneCode\":\"10001\"}]}";
    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);
    ecologicalRuleMgrServiceClient->SetRuleInfo(ruleInfo);

    int32_t res = ecologicalRuleMgrServiceClient->QueryStartExperience(want, callerInfo, rule);
    ASSERT_TRUE(rule.isAllow);
    EXPECT_STREQ("10001", rule.sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);
    GTEST_LOG_(INFO) << "QueryStartExperience_Test2 end";
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

    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    std::string ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    std::string sceneExperience =
        "{\"sceneExperience\":{\"1\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"deviceId\\\":"
        "\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\","
        "\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.LOAD_ABILITY\\\",\\\"entities\\\":null,"
        "\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{\\\\\\\"5\\\\\\\":"
        "\\\\\\\"1002\\\\\\\"}}\\\"}\"}},\"2\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{"
        "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\","
        "\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"2002\\\\\\\"}}\\\"}\"}},\"5\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,"
        "\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":"
        "\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"5002\\\\\\\"}}\\\"}\"}}}}";
    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);

    int32_t res = ecologicalRuleMgrServiceClient->IsSupportPublishForm(wants, callerInfo, bSupport);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // callerInfo包名为空，默认支持加桌
    callerInfo.packageName = "";
    res = ecologicalRuleMgrServiceClient->IsSupportPublishForm(wants, callerInfo, bSupport);
    ASSERT_TRUE(bSupport);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // addCardOwners 没有目标包名，默认支持加桌
    ruleConfig = "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{}]}}";
    sceneExperience =
        "{\"sceneExperience\":{\"1\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"deviceId\\\":"
        "\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\","
        "\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.LOAD_ABILITY\\\",\\\"entities\\\":null,"
        "\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{\\\\\\\"5\\\\\\\":"
        "\\\\\\\"1002\\\\\\\"}}\\\"}\"}},\"2\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{"
        "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\","
        "\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"2002\\\\\\\"}}\\\"}\"}},\"5\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,"
        "\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":"
        "\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"5002\\\\\\\"}}\\\"}\"}}}}";

    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);
    res = ecologicalRuleMgrServiceClient->IsSupportPublishForm(wants, callerInfo, bSupport);

    ASSERT_TRUE(bSupport);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // ruleConfig数据格式异常
    ruleConfig = "{\"configVersion\":\"1\"}";
    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);

    res = ecologicalRuleMgrServiceClient->IsSupportPublishForm(wants, callerInfo, bSupport);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // ruleConfig数据格式异常
    ruleConfig = "{\"configVersion\":\"1\",\"ruleConfig\":{}}";
    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);

    res = ecologicalRuleMgrServiceClient->IsSupportPublishForm(wants, callerInfo, bSupport);
    ASSERT_TRUE(bSupport);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

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
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);
    GTEST_LOG_(INFO) << "EvaluateResolveInfos_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, SetAppEnhancedData_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAppEnhancedData_Test start";


    // full add
    int32_t operType = OHOS::EcologicalRuleMgrService::EngineCacheConstants::APP_ENHANCED_DATA_ALL_ADD;
    std::string appData = "com.myapplication.demo,{\"packageName\":\"com.myapplication.demo\",\"tagInfo\":{\"appType\":"
                          "2,\"blackListType\":2,\"certificationType\":1}}";
    // no permission
    int32_t res = ecologicalRuleMgrServiceClient->SetAppEnhancedData(operType, appData);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // add permission
    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    // part add
    operType = OHOS::EcologicalRuleMgrService::EngineCacheConstants::APP_ENHANCED_DATA_PART_ADD;
    appData = "com.myapplication.test,{\"packageName\":\"com.myapplication.test\",\"tagInfo\":{\"appType\":2,"
              "\"blackListType\":2,\"certificationType\":2}}";
    res = ecologicalRuleMgrServiceClient->SetAppEnhancedData(operType, appData);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // delete
    operType = OHOS::EcologicalRuleMgrService::EngineCacheConstants::APP_ENHANCED_DATA_DEL;
    appData = "com.myapplication.test,{\"packageName\":\"com.myapplication.test\",\"tagInfo\":{\"appType\":2,"
              "\"blackListType\":2,\"certificationType\":2}}";
    res = ecologicalRuleMgrServiceClient->SetAppEnhancedData(operType, appData);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "SetAppEnhancedData_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, SetRuleInfo_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetRuleInfo_Test start";
    std::string ruleInfo = "{\"dataVersion\":\"1\",\"list\":[{\"condition\":\"OR(IS_JUMP_TYPE(caller,want,\\\"2\\\"),"
                           "EQUAL(BUNDLENAME(want),\\\"com.myapplication.demo\\\"))\",\"experience\":{\"load\":2,"
                           "\"open\":2,\"out\":1},\"sceneCode\":\"10000\"}]}";
    // no permission
    int32_t res = ecologicalRuleMgrServiceClient->SetRuleInfo(ruleInfo);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // add permission
    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    res = ecologicalRuleMgrServiceClient->SetRuleInfo(ruleInfo);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // error rule data, the format is not json
    ruleInfo = "OR(IS_JUMP_TYPE(caller,want,\\\"2\\\"),EQUAL(BUNDLENAME(want),\\\"com.myapplication.demo\\\"))";
    res = ecologicalRuleMgrServiceClient->SetRuleInfo(ruleInfo);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // error rule data, missing json key: "dataVersion"
    ruleInfo = "{\"list\":[{\"condition\":\"OR(IS_JUMP_TYPE(caller,want,\\\"2\\\"),EQUAL(BUNDLENAME(want),\\\"com."
               "myapplication.demo\\\"))\",\"experience\":{\"load\":2,\"open\":2,\"out\":1},\"sceneCode\":\"10000\"}]}";
    res = ecologicalRuleMgrServiceClient->SetRuleInfo(ruleInfo);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // error rule data, missing json key: "list"
    ruleInfo =
        "{\"dataVersion\":\"1\",\"rule\":[{\"condition\":\"OR(IS_JUMP_TYPE(caller,want,\\\"2\\\"),EQUAL(BUNDLENAME("
        "want),\\\"com.myapplication.demo\\\"))\",\"experience\":{\"load\":2,\"open\":2},\"sceneCode\":\"10000\"}]}";
    res = ecologicalRuleMgrServiceClient->SetRuleInfo(ruleInfo);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "SetRuleInfo_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, ExemptExperience_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExemptExperience_Test start";

    std::string targetBundleName = "com.myapplication.demo";
    int32_t rule = 1;
    int32_t timestamp = 169180;

    // no permission
    int32_t res = ecologicalRuleMgrServiceClient->ExemptExperience(targetBundleName, rule, timestamp);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // add permission
    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    res = ecologicalRuleMgrServiceClient->ExemptExperience(targetBundleName, rule, timestamp);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "ExemptExperience_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, GetVersion_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetVersion_Test start";

    std::string versionJson;
    std::string target = "{\"engineVersion\":1,\"dataVersion\":1,\"configVersion\":1}";

    // no permission
    int32_t res = ecologicalRuleMgrServiceClient->GetVersion(versionJson);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // add permission
    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    res = ecologicalRuleMgrServiceClient->GetVersion(versionJson);
    EXPECT_STREQ(target.c_str(), versionJson.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "GetVersion_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, SetSceneExperience_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSceneExperience_Test start";
    std::string ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    std::string sceneExperience =
        "{\"sceneExperience\":{\"1\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"deviceId\\\":"
        "\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\","
        "\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.LOAD_ABILITY\\\",\\\"entities\\\":null,"
        "\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{\\\\\\\"5\\\\\\\":"
        "\\\\\\\"1002\\\\\\\"}}\\\"}\"}},\"2\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{"
        "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\","
        "\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"2002\\\\\\\"}}\\\"}\"}},\"5\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,"
        "\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":"
        "\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"5002\\\\\\\"}}\\\"}\"}}}}";

    // no permission
    int32_t res = ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // add permission
    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    res = ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // error ruleConfig data, the format is not json
    ruleConfig = "\"configVersion\": \"1\"";
    res = ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // error ruleConfig json data, missing key: "configVersion"
    ruleConfig = "{\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    res = ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // error sceneExperience json data, missing key: "sceneExperience"
    ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    sceneExperience =
        "{\"experience\":{\"1\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"deviceId\\\":\\\"\\\","
        "\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\","
        "\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.LOAD_ABILITY\\\",\\\"entities\\\":null,"
        "\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{\\\\\\\"5\\\\\\\":"
        "\\\\\\\"1002\\\\\\\"}}\\\"}\"}},\"2\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{"
        "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\","
        "\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"2002\\\\\\\"}}\\\"}\"}},\"5\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,"
        "\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":"
        "\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"5002\\\\\\\"}}\\\"}\"}}}}";
    res = ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "SetSceneExperience_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, GetSceneCode_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSceneCode_Test start";

    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_HARMONY_APP;
    callerInfo.targetAppType = CallerInfo::TYPE_ATOM_SERVICE;
    ExperienceRule rule;
    ecologicalRuleMgrServiceClient->QueryFreeInstallExperience(want, callerInfo, rule);

    std::string bundleName = "com.myapplication.demo";
    std::string sceneCode;

    // no permission
    int32_t res = ecologicalRuleMgrServiceClient->GetSceneCode(bundleName, sceneCode);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // add permission
    const char *perms[] = {PERMISSION_GET_SCENE_CODE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    res = ecologicalRuleMgrServiceClient->GetSceneCode(bundleName, sceneCode);
    EXPECT_STREQ("10001", sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // no sceneCode
    bundleName = "com.myapplication.test";
    res = ecologicalRuleMgrServiceClient->GetSceneCode(bundleName, sceneCode);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "GetSceneCode_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, GetInitialSceneCode_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetInitialSceneCode_Test start";

    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_HARMONY_APP;
    callerInfo.targetAppType = CallerInfo::TYPE_ATOM_SERVICE;
    ExperienceRule rule;
    ecologicalRuleMgrServiceClient->QueryStartExperience(want, callerInfo, rule);

    std::string bundleName = "com.myapplication.demo";
    std::string sceneCode;

    // no permission
    int32_t res = ecologicalRuleMgrServiceClient->GetInitialSceneCode(bundleName, sceneCode);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // add permission
    const char *perms[] = {PERMISSION_GET_SCENE_CODE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    res = ecologicalRuleMgrServiceClient->GetInitialSceneCode(bundleName, sceneCode);
    EXPECT_STREQ("10000", sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    // no sceneCode
    bundleName = "com.myapplication.test";
    res = ecologicalRuleMgrServiceClient->GetSceneCode(bundleName, sceneCode);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "GetInitialSceneCode_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, GetSelfSceneCode_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSelfSceneCode_Test start";
    std::string sceneCode;
    int32_t res = ecologicalRuleMgrServiceClient->GetSelfSceneCode(sceneCode);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);
    GTEST_LOG_(INFO) << "GetSelfSceneCode_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, GetAdsVerificationVersion_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAdsVerificationVersion_Test start";
    int32_t version;
    int32_t res = ecologicalRuleMgrServiceClient->GetAdsVerificationVersion(version);
    EXPECT_EQ(1, version);
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);
    GTEST_LOG_(INFO) << "GetAdsVerificationVersion_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, ERROR_DSL_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ERROR_DSL_Test start";
    // ERROR DSL
    std::string ruleInfo =
        "{\"dataVersion\":\"1\",\"list\":[{\"condition\":\"OR(\\\"TRUE\\\",IS_JUMP_TYPE(caller,want,\\\"2\\\"))\","
        "\"experience\":{\"load\":2,\"open\":2,\"out\":1},\"sceneCode\":\"10000\"},{\"condition\":\"AND(IS_JUMP_TYPE("
        "AA,BB,\\\"2\\\"),\\\"TRUE\\\")\",\"experience\":{\"load\":2,\"open\":2,\"out\":1},\"sceneCode\":\"10001\"},{"
        "\"condition\":\"AND(\\\"TRUE\\\",\\\"TRUE\\\")\",\"experience\":{\"load\":2,\"open\":2,\"out\":1},"
        "\"sceneCode\":\"10002\"},{\"condition\":\"OR(IS_JUMP_TYPE(caller,want,\\\"1\\\"),\\\"TRUE\\\")\","
        "\"experience\":{\"load\":2,\"open\":2,\"out\":1},\"sceneCode\":\"10003\"},{\"condition\":\"or(IS_JUMP_TYPE("
        "caller,want,\\\"1\\\"),\\\"TRUE\\\")\",\"experience\":{\"load\":2,\"open\":2,\"out\":1},\"sceneCode\":"
        "\"10004\"}]}";

    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    ecologicalRuleMgrServiceClient->SetRuleInfo(ruleInfo);

    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_HARMONY_APP;
    callerInfo.targetAppType = CallerInfo::TYPE_ATOM_SERVICE;
    ExperienceRule rule;

    int32_t res = ecologicalRuleMgrServiceClient->QueryFreeInstallExperience(want, callerInfo, rule);
    ASSERT_TRUE(rule.isAllow);
    EXPECT_STREQ("00000", rule.sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "ERROR_DSL_Test end";
}

HWTEST_F(EcologicalRuleMgrServiceClientTest, IS_JUMP_TYPE_Test, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IS_JUMP_TYPE_Test start";

    std::string ruleInfo = "{\"dataVersion\":\"1\",\"list\":[{\"condition\":\"AND(EQUAL(BUNDLENAME(want),\\\"com."
                           "myapplication.demo\\\"),IS_JUMP_TYPE(caller,want,\\\"1\\\"))\",\"experience\":{\"load\":2,"
                           "\"open\":2,\"out\":1},\"sceneCode\":\"10003\"}]}";
    std::string ruleConfig =
        "{\"configVersion\":\"1\",\"ruleConfig\":{\"addCardOwners\":[{\"pkgName\":\"com.myapplication.demo\"}]}}";
    std::string sceneExperience =
        "{\"sceneExperience\":{\"1\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{\\\"deviceId\\\":"
        "\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\",\\\"type\\\":\\\"\\\","
        "\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.LOAD_ABILITY\\\",\\\"entities\\\":null,"
        "\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{\\\\\\\"5\\\\\\\":"
        "\\\\\\\"1002\\\\\\\"}}\\\"}\"}},\"2\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,\"want\":\"{"
        "\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":\\\"\\\","
        "\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"2002\\\\\\\"}}\\\"}\"}},\"5\":{\"1\":{\"allow\":true},\"2\":{\"allow\":false,"
        "\"want\":\"{\\\"deviceId\\\":\\\"\\\",\\\"bundleName\\\":\\\"\\\",\\\"abilityName\\\":\\\"\\\",\\\"uri\\\":"
        "\\\"\\\",\\\"type\\\":\\\"\\\",\\\"flags\\\":0,\\\"action\\\":\\\"com.ecological.experience.OPEN_ABILITY\\\","
        "\\\"entities\\\":null,\\\"parameters\\\":\\\"{\\\\\\\"ecological_experience_open_type\\\\\\\":{"
        "\\\\\\\"5\\\\\\\":\\\\\\\"5002\\\\\\\"}}\\\"}\"}}}}";

    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    ecologicalRuleMgrServiceClient->SetRuleInfo(ruleInfo);
    ecologicalRuleMgrServiceClient->SetSceneExperience(ruleConfig, sceneExperience);

    // 直接跳出场景
    Want want;
    want.SetBundle("com.myapplication.demo");
    CallerInfo callerInfo;
    callerInfo.packageName = "com.myapplication.test";
    callerInfo.callerAppType = CallerInfo::TYPE_ATOM_SERVICE;
    callerInfo.targetAppType = CallerInfo::TYPE_HARMONY_APP;
    ExperienceRule rule;

    int32_t res = ecologicalRuleMgrServiceClient->QueryStartExperience(want, callerInfo, rule);
    ASSERT_TRUE(rule.isAllow);
    EXPECT_STREQ("10003", rule.sceneCode.c_str());
    ASSERT_EQ(OHOS::EcologicalRuleMgrService::ExpReturnCode::SUCCESS, res);

    GTEST_LOG_(INFO) << "IS_JUMP_TYPE_Test end";
}
}
}