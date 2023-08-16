# bundlemanager_ecological_rule_manager

###Introduction
To ensure that the AtomicService ecosystem is controllable, orderly, and efficient, the behavior and results of the AtomicService must be controlled to ensure the orderly development of the ecosystem, ensure user experience, and protect the rights and interests of developers. Therefore, the ecological rule management service (EcologicalRuleManagerService, ERMS) of AtomicService is constructed. The ecological rule management service is a new component under the OpenHarmony Bundle management subsystem. The service is SystemAbility, that is, System Meta Capability, SA for short, is a System Service. The ecosystem rule management service stores rule data delivered by RuleMgrApp. When a AtomicService is loaded, started, or added to desktop, the ecosystem rule management service identifies the corresponding scenario based on the rule data and returns the user experience (allowed, forbidden, and pop-up confirmation) corresponding to the scenario.


### Software Architecture
![image](figures/architecture.jpg)

The following describes the key fields in the preceding figure:

- `RuleMgrApp`: rule management app, which is responsible for maintaining management and control rules, experience, and app data, and updating related data in the rule management service.
- `Third App`: third-party app, which is controlled by the ecosystem rule service when the AtomicService is opened.
- `AbilityManagerService`: a system service used to coordinate the running relationships of capabilities and schedule life cycles.
- `EcologicalRuleManagerService`: Caches data such as delivered rules, parses and matches rules, and provides related interfaces for service.
- `AbilityManager`: indicates the capability management SDK, which is provided by the capability management service.
- `EcologicalRuleManager`: ecosystem rule management service SDK, which is provided by the ERMS SA.

### Directory Structure

```shell
/foundation/bundlemanager/ecological_rule_mgr       # EcologicalRuleManagerService code
├── etc                                             
│   └── init                                        # SA startup configuration file
├── interfaces                                      # Interface Code
│   ├── innerkits                                   # Internal interface
│   └── kits                                        # External interface
├── profile                                         # Service Profile
├── services                                        # Service Code
├── LICENSE                                         # Certificate file
├── tests                                           # Developer test
└── utils                                           # Tools
```
### build

In the root directory of the OpenHarmony source code, invoke the following command to compile ecological_rule_mgr:
```shell
./build.sh --product-name rk3568 --ccache --build-target ecological_rule_mgr
```
> **Note: **
--product-name: product name, for example, Hi3516D V300 or rk3568.
--ccache: The cache function is used during compilation.
--build-target: name of the compiled component.

### Interface Description
##### Interfaces provided for apps
> refer [Interface Description](https://gitee.com/openharmony/docs/pulls/21544)

##### Interfaces Opened to OpenHarmony AbilityManagerService
|Interface Name|Interface Description|
|---|---|
|QueryStartExperience(const AAFwk::Want &want, const CallerInfo &callerInfo, ExperienceRule &rule): int32_t|This interface is used to query the experience of starting a AtomicService. callerInfo indicates the information about the caller, and want indicates the information about the AtomicService to be started. rule is an output parameter. Whether to enable the startup is returned or a dialog box is displayed for users to confirm.|
|QueryFreeInstallExperience(const AAFwk::Want &want, const CallerInfo &callerInfo, ExperienceRule &rule): int32_t|This interface is used to query the installation-free experience of AtomicService. callerInfo indicates the caller information, and want indicates the installation-free AtomicService information. rule is an output parameter, indicating whether installation is not allowed or whether a dialog box is displayed for users to confirm.|
|IsSupportPublishForm(const vector<AAFwk::Want> &wants, const CallerInfo &callerInfo, bool &bSupport): int32_t|Queries whether the AtomicService can be added to desktop. callerInfo indicates the information about the caller, and want indicates the information about the AtomicService to be added to desktop. bSupport is an output parameter, indicating whether to add desktop.|

### Instructions
The rule management service needs to develop RuleMgrApp and deliver ecosystem rule management policies to EcologicalRuleManagerService. In this way, AtomicService can be loaded, started, and added to desktop. The main steps are as follows:
#### 1 Develop the RuleMgrApp and apply for the ohos.permission.MANAGE_ECOLOGICAL_RULE permission
#### 2 Update ecosystem management policies
Deliver rule policy data, scenario experience, and app enhancement data to EcologicalRuleManagerService.
##### 2.1 Deliver rule policy data
Rule policy data is used to describe scenario values and corresponding management and control policies. It is described in the DSL language and supports operator extension. The rule information is delivered through the function setRuleInfo(ruleInfo: string): number interface. The ruleInfo is in JSON format and describes the policy information.
###### Example（ruleInfo: string）
Deliver two rules:
- rule 1
sceneCode: 10000
matching rule: The package name of the want is com.myapplication.test, or jumps out of the AtomicService.
experience: If the action is load, the experience action:"com.ecological.experience.LOAD_ABILITY" is executed, which is defined and processed by the RuleMgrApp. If it is pulled up, it is allowed directly.
- rule 2
sceneCode: 10001
matching rule: The package name of the caller is com.myapplication.test and jump-in AtomicService.
experience: If the action is load, it is allowed directly. If the action is open, the experience action:"com.ecological.experience.OPEN_ABILITY" is executed, which is defined and processed by the RuleMgrApp.

Corresponding ruleInfo:
```
{
	"dataVersion": "1",   
	"list": [
        {
		"condition": "OR(EQUAL(BUNDLENAME(want),\"com.myapplication.test\"),
                         IS_JUMP_TYPE(caller,want,\"1\"))",
		"experience": {
			"load": 2,
			"open": 1
		},
		"sceneCode": "10000"
	    }, 
        {
		"condition": "AND(IS_JUMP_TYPE(caller,want,\"2\"),
                          EQUAL(BUNDLENAME(caller),\"com.myapplication.test\"))",
		"experience": {
			"load": 1,
			"open": 2
		},
		"sceneCode": "10001"
	}]
}

```
|Keyword|Meaning|
|---|---|
|dataVersion|Version number of the rule data|
|List|Rule data list,multiple rule data records are supported.|
|condition|Rule DSL description. Currently, four operators are supported: OR, Equal, IS_JUMP_TYPE, and BUNDLENAME. You can extend the operators as required.|
|experience|After the rule data is matched, if the action is load, experience 2 is returned. if the action is open, experience 1 is returned. load, open corresponds to the key in sceneExperience, load corresponds to 1, and open corresponds to 2. The value of load and open corresponds to the key of the lower level.|
|sceneCode|SceneCode corresponding to the rule.|

##### 2.2 Deliver Scenario Experience：
Scenario experience description: experience configuration corresponding to the scenario value and configuration of the list of packages that can be added to desktop. The function setSceneExperience(ruleConfig: string, sceneExperience: string): number interface is invoked. The values of sceneExperience and ruleConfig are in JSON format. sceneExperience describes the experience, and ruleConfig describes the name of the app package that can be added to the desktop.

###### Example 1（sceneExperience: string）
```
{
	"sceneExperience": {
		"1": {
			"1": {"allow": true},
			"2": {"allow": false,
				  "want": 
                    "{ 
                        \"deviceId\":\"\",
                        \"bundleName\":\"\",
                        \"abilityName\":\"\",
                        \"uri\":\"\",
                        \"type\":\"\",
                        \"flags\":0,
                        \"action\":\"com.ecological.experience.LOAD_ABILITY\",        
                        \"entities\":null,
                        \"parameters\":
                            \"{\\\"ecological_experience_open_type\\\":
                                {\\\"5\\\":\\\"1002\\\"}
                            }\"
                    }"
			    }
		},
		"2": {
			"1": {"allow": true},
			"2": {"allow": false,
				  "want":
                    "{
                        \"deviceId\":\"\",
                        \"bundleName\":\"\",
                        \"abilityName\":\"\",
                        \"uri\":\"\",
                        \"type\":\"\",
                        \"flags\":0,
                        \"action\":\"com.ecological.experience.OPEN_ABILITY\",
                        \"entities\":null,
                        \"parameters\":
                            \"{\\\"ecological_experience_open_type\\\":
                                {\\\"5\\\":\\\"2002\\\"}
                            }\"
                    }"
			    }
		}
	}
}
```
|Keyword|Meaning|
|---|---|
|"sceneExperience": {"1": {}，"2": {}}|Experience configuration corresponding to the scenario value. The value 1 indicates load, which corresponds to load in ruleInfo. The value 2 indicates start, corresponding to open in ruleInfo. Developers can extend the value as required.|
|"allow": true|allow directly |
|want|The experience information which corresponds to sceneCode, for example, pop-up window.|

###### Example 2（ruleConfig: string）
In ruleConfig, addCardOwners indicates the application that can be added to desktop, and value indicates the package name of the application.
In the following example, addCardOwners contains com.app.launcher, indicating that com.app.launcher has the permission to add to desktop and allows com.app.launcher to invoke the desktop adding interface to add a AtomicService to desktop.
```	
{"ruleConfig": {
		"addCardOwners": [{
			"pkgName": "com.app.launcher"
		}, {
			"pkgName": "com.app.market"
		}]
    }
}

```
##### 2.3 Deliver App Enhancement Data:
App enhancement data is an extended interface, which can be implemented by developers as required.
Interface Definition：function setAppEnhancedDatas(operType: number, appData: string):：number。
This interface is used to extend rule instructions. Some app attribute information is required in some scenarios. For example, if you need to know which apps are in the blacklist for rule determination, this interface can be used to deliver the app blacklist.