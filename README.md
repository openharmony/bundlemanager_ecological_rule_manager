# bundlemanager_ecological_rule_manager

### 简介
为了确保元服务的生态是可控、有序、高效的，需要对元服务的行为、结果进行管控，确保生态的有序发展，保证用户体验的同时也保障开发者的权益， 为此构建了元服
务的生态规则管控服务(EcologicalRuleManagerService), 生态规则管控服务是OpenHarmony包管理子系统下新增的部件，该服务是SystemAbility， 即系
统元能力，简称SA, 是一种系统服务。

### 软件架构
![image](figures/architecture.jpg)

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

|接口名称|接口描述|
|---|---|
|SetAppEnhancedData(const int32_t &operType, const std::string &appData): int32_t|刷新应用的增强数据。|
|SetRuleInfo(const std::string &ruleInfo): int32_t|刷新生态规则策略。|
|ExemptExperience(const std::string &targetBundleName, const int32_t &rule, const int32_t &timestamp): int32_t|短时间内豁免目标应用。|
|GetVersion(std::string &versionJson): int32_t|查询引擎版本号信息。|
|SetSceneExperience(std::string &ruleConfig, std::string &sceneExperience): int32_t|设置场景值的体验配置。|
|GetSceneCode(const std::string &bundleName, std::string &sceneCode): int32_t|获取指定应用的场景值。|
|GetInitialSceneCode(const std::string &bundleName, std::string &originalSceneCode): int32_t|获取指定应用的场景值。|
|GetSelfSceneCode(std::string &sceneCode): int32_t|获取自身应用的场景值。|
|GetAdsVerificationVersion(int32_t &version): int32_t|获取广告验签的版本。|

### 贡献代码

使用过程中发现任何问题都可以提 [Issue](https://gitee.com/openharmony-sig/bundlemanager_ecological_rule_manager/issues)
给我们，当然，我们也非常欢迎你给我们发 [PR](https://gitee.com/openharmony-sig/bundlemanager_ecological_rule_manager/pulls) 。

### 开源协议

本项目基于 [Apache License 2.0 ](https://gitee.com/openharmony-sig/bundlemanager_ecological_rule_manager/blob/master/LICENSE) ，请自由地享受和参与开源。
