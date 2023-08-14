# bundlemanager_ecological_rule_manager

### 简介
为了确保元服务的生态是可控、有序、高效的，需要对元服务的行为、结果进行管控，确保生态的有序发展，保证用户体验的同时也保障开发者的权益， 为此构建了元服务的生态规则管控服务(EcologicalRuleManagerService), 生态规则管控服务是OpenHarmony包管理子系统下新增的部件，该服务是SystemAbility， 即系统元能力，简称SA, 是一种系统服务。生态规则管控服务负责存储RuleMgrApp下发的规则数据，在元服务加载，拉起，加桌时，根据规则数据，识别对应的场景，并返回场景对应的用户体验（允许，禁止，弹窗确认等）。


### 软件架构
![image](figures/architecture.jpg)

以下是对于上图关键字段的描述信息：

- `RuleMgrApp`：规则管控APP，负责管控规则，体验，APP数据的维护，刷新规则管控服务中相关数据。
- `Third App`：三方App，在打开元服务时会受到生态规则服务的管控。
- `AbilityManagerService`：用于协调各Ability运行关系、及对生命周期进行调度的系统服务。
- `EcologicalRuleManagerServie`：对下发的规则、策略等数据进行缓存，规则进行解析匹配，对业务提供相关接口。
- `AbilityManager`：Ability管理SDK，由Ability管理服务提供。
- `EcologicalRuleManager`：生态规则管理服务SDK，由ERMS SA提供。


### 目录结构

```shell
/foundation/bundlemanager/ecological_rule_mgr       # 生态规则管控服务业务代码
├── etc                                             
│   └── init                                        # SA启动配置文件
├── interfaces                                      # 接口代码
│   ├── innerkits                                   # 内部接口
│   └── kits                                        # 外部接口
├── profile                                         # 服务配置文件
├── services                                        # 服务代码
├── LICENSE                                         # 证书文件
├── tests                                           # 开发者测试
└── utils                                           # 工具类
```
### 编译构建

在OpenHarmony源码根目录下，调用以下指令，单独编译ecological_rule_mgr。
```shell
./build.sh --product-name rk3568 --ccache --build-target ecological_rule_mgr
```
> **说明：**
--product-name：产品名称，例如Hi3516DV300、rk3568等。
--ccache：编译时使用缓存功能。
--build-target: 编译的部件名称。


### 接口说明
##### 对APP提供的接口
> 参见[接口说明](https://gitee.com/openharmony-sig/)

##### 对OpenHarmoby AbilityManagerService开放的接口
|接口名称|接口描述|
|---|---|
|QueryStartExperience(const AAFwk::Want &want, const CallerInfo &callerInfo, ExperienceRule &rule): int32_t|查询元服务拉起体验。callerInfo为调用者信息，want为被拉起元服务信息；rule为出参，返回是否允许拉起，还是弹窗由用户确认|
|QueryFreeInstallExperience(const AAFwk::Want &want, const CallerInfo &callerInfo, ExperienceRule &rule): int32_t|查询元服务免安装体验。callerInfo为调用者信息，want为免安装元服务信息；rule为出参，返回是否允许免安装，还是弹窗由用户确认|
|IsSupportPublishForm(const vector<AAFwk::Want> &wants, const CallerInfo &callerInfo, bool &bSupport): int32_t|查询元服务是否允许加桌。callerInfo为调用者信息，want为加桌的元服务信息；bSupport为出参，返回是否允许加桌。|

### 使用说明
规则管控服务需要开发RuleMgrApp，下发生态规则管控策略到EcologicalRuleManagerServie，才能在元服务的加载，拉起，加桌时起到管控的作用。主要有如下几步：
#### 1 开发RuleMgrApp，申请权限 ohos.permission.MANAGE_ECOLOGICAL_RULE
#### 2 刷新生态管控策略
下发规则策略数据，体验数据，规则配置数据，APP增强数据到EcologicalRuleManagerServie：
##### 2.1 下发规则策略数据
规则策略数据用于描述场景值和对应的管控策略，采用DSL语言描述，支持对操作符的扩展。通过接口 function setRuleInfo(ruleInfo: string): number 下发，ruleInfo为json格式，描述策略信息。
###### 示例（ruleInfo: string）
下发两个策略：
- 策略1
场景值：10000；
匹配规则：被调用方的包名是com.myapplication.test，或者从元服务中跳出；
场景体验：如果是加载，则执行体验 action:"com.ecological.experience.LOAD_ABILITY"，由RuleMgrApp定义并处理该action；如果是拉起，则直接允许。
- 策略2
场景值：10001；
匹配规则：调用方的包名是com.myapplication.test，并且是跳入元服务；
场景体验：加载元服务则直接允许；打开元服务则对应第2个体验，action:"com.ecological.experience.OPEN_ABILITY"，，由RuleMgrApp定义并处理该action。

对应ruleInfo：
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
|关键字|含义|
|---|---|
|dataVersion|规则数据的版本号|
|List|规则数据列表，支持多条规则数据|
|condition|规则DSL描述，当前支持 OR，EQUAL，IS_JUMP_TYPE，BUNDLENAME四种操作符，可以根据自己需要扩展|
|experience|标识匹配该规则数据后，如果是加载（load），返回体验2；拉起（open），返回体验1；load，open 对应sceneExperience中的key, load对应"1", open对应"2"；load，open的取值对应下一级的key|
|sceneCode|该规则对应的场景值|

##### 2.2 下发场景体验：
场景体验描述场景值对应的体验配置和允许加桌名单配置。通过接口 function setSceneExperience(ruleConfig: string, sceneExperience: string): number 下发，sceneExperience，ruleConfig均为json格式，sceneExperience描述体验，ruleConfig描述允许加桌的app包名。

###### 示例1（sceneExperience: string）
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
|关键字|含义|
|---|---|
|"sceneExperience": {"1": {}，"2": {}}|场景值对应的体验配置：1表示加载，对应规则ruleInfo信息experience中的 load；2表示拉起，对应规则ruleInfo信息experience中的 open；开发者可以根据自己的实际需要对此值进行扩展。|
|"allow": true|直接允许|
|want|场景值对应体验信息，可以为弹窗等|

###### 示例2（ruleConfig: string）
ruleConfig中addCardOwners标识允许执行加桌动作的应用，value为应用对应的包名。
如下的addCardOwners中包含了com.app.launcher，表示com.app.launcher具有加桌的权限，允许
com.app.launcher调用加桌接口将元服务加桌。
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
##### 2.3 下发APP增强数据：
APP增强数据属于扩展接口，开发者可以根据需要自行实现。
接口定义：function setAppEnhancedDatas(operType: number, appData: string):：number。
该接口主要考虑是在扩展规则指令时，某些场景需要用到app的一些属性信息，比如规则判断如果需要知道哪些APP是黑名单中的APP，可以通过此接口下发APP黑名单列表。
