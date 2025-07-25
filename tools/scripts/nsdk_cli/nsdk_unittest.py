from nsdk_utils import *
import unittest
import os

class TestFilterAppConfig(unittest.TestCase):
    def setUp(self):
        """设置测试环境"""
        if "SDK_SUPPORT_ARCH" in os.environ:
            del os.environ["SDK_SUPPORT_ARCH"]
        if "SDK_IGNORED_EXTS" in os.environ:
            del os.environ["SDK_IGNORED_EXTS"]

    def test_invalid_input(self):
        """测试无效输入"""
        self.assertEqual(filter_app_config(None), (False, ""))
        self.assertEqual(filter_app_config({}), (False, ""))
        self.assertEqual(filter_app_config({"no_build_config": {}}), (False, ""))

    def test_empty_build_config(self):
        """测试空的构建配置"""
        config = {"build_config": {}}
        self.assertEqual(filter_app_config(config), (False, ""))

    def test_arch_compatibility(self):
        """测试架构兼容性检查"""
        # 设置支持的架构
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imafdc"

        # 测试兼容的配置
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": ""
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

        # 测试不兼容的配置
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "v"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)
        self.assertTrue("not supported" in msg)

    def test_ignored_extensions(self):
        """测试被忽略的扩展"""
        os.environ["SDK_IGNORED_EXTS"] = "p_zfh"

        # 测试单字母扩展
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "p"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)
        self.assertTrue("Filtered by p extension" in msg)

        # 测试多字母扩展
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "i_zfh"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)
        self.assertTrue("Filtered by zfh extension" in msg)

    def test_extension_formats(self):
        """测试不同的扩展格式"""
        os.environ["SDK_IGNORED_EXTS"] = "zfh"

        # 测试前导下划线格式
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "_zfh"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)

        # 测试无下划线格式
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "zfh"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)

    def test_multiple_ignored_extensions(self):
        """测试多个被忽略的扩展"""
        os.environ["SDK_IGNORED_EXTS"] = "p_zfh_zvl128"

        # 测试匹配任意一个扩展
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "i_zvl128"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)

    def test_no_ignored_extensions(self):
        """测试未设置忽略扩展的情况"""
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "p_zfh"
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

    def test_complex_arch_scenarios(self):
        """测试复杂的架构场景"""
        # 测试RV64
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdc"
        config = {
            "build_config": {
                "CORE": "n900",
                "ARCH_EXT": "v_zfh"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)

        # 测试带版本号的扩展
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imac_zicsr_zifencei"
        config = {
            "build_config": {
                "CORE": "n300",
                "ARCH_EXT": "zicsr_zifencei"
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

    def test_invalid_arch_formats(self):
        """测试无效的架构格式"""
        # 测试无效的XLEN
        os.environ["SDK_SUPPORT_ARCH"] = "rv128imac"
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": ""
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

        # 测试非法架构字符串
        os.environ["SDK_SUPPORT_ARCH"] = "invalid_arch"
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": ""
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

    def test_special_extensions(self):
        """测试特殊扩展和扩展间的关系"""
        # 测试vector扩展的隐含关系
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdcv"
        config = {
            "build_config": {
                "CORE": "nx900fd",
                "ARCH_EXT": "v_zvl128b"
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

        # 测试DSP扩展的隐含关系
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imafc_xxldspn3x"
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "p_dspn3"
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

    def test_core_configurations(self):
        """测试不同的CORE配置场景"""
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imac"

        # 测试不存在的CORE
        config = {
            "build_config": {
                "CORE": "nonexist",
                "ARCH_EXT": ""
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

        # 测试没有CORE配置
        config = {
            "build_config": {
                "ARCH_EXT": "v"
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

    def test_mixed_extensions(self):
        """测试混合扩展场景"""
        # 测试扩展组合
        os.environ["SDK_IGNORED_EXTS"] = "zfh_zvl256b"
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "v_zfh_zvl256b"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)

        # 测试重复扩展
        os.environ["SDK_IGNORED_EXTS"] = "p_p_zfh"
        config = {
            "build_config": {
                "CORE": "n300f",
                "ARCH_EXT": "p"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)

    def test_200series_cores(self):
        """测试200系列处理器核心"""
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imac"

        # 测试基本RV32E核心
        config = {
            "build_config": {
                "CORE": "n200e",
                "ARCH_EXT": ""
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

        # 测试带A扩展的核心
        config["build_config"]["CORE"] = "n201"
        self.assertEqual(filter_app_config(config), (False, ""))

        # 测试不兼容的扩展
        config = {
            "build_config": {
                "CORE": "n203",
                "ARCH_EXT": "v"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)

    def test_300series_cores(self):
        """测试300系列处理器核心"""
        # 测试浮点扩展兼容性
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imafdc"

        config = {
            "build_config": {
                "CORE": "n300fd",
                "ARCH_EXT": ""
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

        # 测试单精度浮点
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imafc"
        config["build_config"]["CORE"] = "n300f"
        self.assertEqual(filter_app_config(config), (False, ""))

    def test_600series_cores(self):
        """测试600系列处理器核心"""
        # 测试RV64核心
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdc"

        config = {
            "build_config": {
                "CORE": "nx600fd",
                "ARCH_EXT": ""
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

        # 测试RV64核心的向量扩展
        config = {
            "build_config": {
                "CORE": "ux600fd",
                "ARCH_EXT": "v"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)

    def test_900series_cores(self):
        """测试900系列处理器核心"""
        # 测试RV64核心与向量扩展
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdcv"

        config = {
            "build_config": {
                "CORE": "nx900fd",
                "ARCH_EXT": "v_zvl512b"
            }
        }
        filtered, msg = filter_app_config(config)
        self.assertTrue(filtered)

        # 测试RV32核心
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imafdc"
        config = {
            "build_config": {
                "CORE": "n900fd",
                "ARCH_EXT": ""
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

    def test_1000series_cores(self):
        """测试1000系列处理器核心"""
        # 测试RV64核心
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdc_zfh_zvk_zvb"

        config = {
            "build_config": {
                "CORE": "nx1000fd",
                "ARCH_EXT": "_zfh_zvbb_zvbc_zvkg_zvkn"
            }
        }
        self.assertEqual(filter_app_config(config), (False, ""))

        # 测试用户级RV64核心
        config["build_config"]["CORE"] = "ux1000fd"
        self.assertEqual(filter_app_config(config), (False, ""))

    def test_vector_extensions(self):
        """测试向量扩展的各种组合"""
        # 测试RV32支持向量扩展
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imafdcv"
        test_cases = [
            ("v", False),                # RV32支持向量扩展
            ("v_zvl128b", False),        # RV32支持向量扩展
            ("v_zve32x", False),         # RV32支持向量扩展
        ]
        for ext, should_filter in test_cases:
            config = {
                "build_config": {
                    "CORE": "n300fd",
                    "ARCH_EXT": ext
                }
            }
            filtered, msg = filter_app_config(config)
            self.assertEqual(filtered, should_filter, f"Failed for extension {ext}")
            if should_filter:
                self.assertTrue("not supported" in msg)

        # 测试RV64支持向量扩展
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdcv"
        test_cases = [
            ("v", False),               # 基本向量扩展
            ("v_zve64d", False),        # 支持双精度向量
            ("v_zvl128b", False),       # 支持128位向量
            ("v_zvl256b", True),        # 不支持256位向量
        ]
        for ext, should_filter in test_cases:
            config = {
                "build_config": {
                    "CORE": "nx900fd",
                    "ARCH_EXT": ext
                }
            }
            filtered, _ = filter_app_config(config)
            self.assertEqual(filtered, should_filter, f"Failed for extension {ext}")

    def test_dsp_extensions(self):
        """测试DSP扩展的各种组合"""
        os.environ["SDK_SUPPORT_ARCH"] = "rv32imacp"

        test_cases = [
            ("p", False),              # 基本P扩展
            ("p_dsp", False),          # DSP扩展
            ("p_dspn1", True),        # DSPN1扩展
            ("_dspn2", True),          # 当前配置不支持DSPN2
            ("_dspn3", True),          # 当前配置不支持DSPN3
        ]

        for ext, should_filter in test_cases:
            config = {
                "build_config": {
                    "CORE": "n300",
                    "ARCH_EXT": ext
                }
            }
            filtered, _ = filter_app_config(config)
            self.assertEqual(filtered, should_filter, f"Failed for extension {ext}")

    def test_k_extensions(self):
        """测试K扩展及其子扩展的组合"""
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdck"

        test_cases = [
            ("k", False),                      # 基本K扩展
            ("k_zkn_zkr", False),             # 支持的K子扩展组合
            ("k_zks_zkt", False),             # 支持的K子扩展组合
            ("k_invalid_ext", True),          # 无效的K扩展
        ]

        for ext, should_filter in test_cases:
            config = {
                "build_config": {
                    "CORE": "nx1000fd",
                    "ARCH_EXT": ext
                }
            }
            filtered, _ = filter_app_config(config)
            self.assertEqual(filtered, should_filter, f"Failed for extension {ext}")

    def test_mixed_extension_combinations(self):
        """测试多种扩展的混合组合"""
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdcvk_zve64d"

        test_cases = [
            ("vk", False),                    # 基本向量+K扩展
            ("vk_zve64d", False),            # 带向量子扩展
            ("vk_invalid", True),            # 无效扩展组合
            ("zc_zicsr", False),              # 基础扩展组合
        ]

        for ext, should_filter in test_cases:
            config = {
                "build_config": {
                    "CORE": "nx1000fd",
                    "ARCH_EXT": ext
                }
            }
            filtered, _ = filter_app_config(config)
            self.assertEqual(filtered, should_filter, f"Failed for extension {ext}")

    def test_extension_compatibility(self):
        """测试扩展的兼容性检查"""
        test_cases = [
            ("rv32imafdc", "n300fd", "v", True),          # RV32不支持向量
            ("rv64imafdcv", "nx900fd", "v", False),       # RV64支持向量
            ("rv32imacp", "n300", "p_dsp", False),        # 支持DSP
            ("rv64imafdck", "nx1000fd", "k_zkn", False),  # 支持K扩展
            ("rv32imac", "n300", "zc", False),            # 支持压缩指令
        ]

        for arch, core, ext, should_filter in test_cases:
            os.environ["SDK_SUPPORT_ARCH"] = arch
            config = {
                "build_config": {
                    "CORE": core,
                    "ARCH_EXT": ext
                }
            }
            filtered, _ = filter_app_config(config)
            self.assertEqual(filtered, should_filter,
                           f"Failed for {arch} {core} with ext {ext}")

    def test_all_cores_basic(self):
        """测试所有核心的基本配置"""
        # 200系列测试
        test_cases = [
            ("n200", "rv32imc"),      # 基本IMC核心
            ("n200e", "rv32emc"),     # 基本EMC核心
            ("n201", "rv32iac"),      # IAC核心
            ("n201e", "rv32eac"),     # EAC核心
            ("n203", "rv32imac"),     # IMAC核心
            ("n205", "rv32imac"),     # IMAC核心
            # 300系列测试
            ("n300", "rv32imac"),     # IMAC核心
            ("n300f", "rv32imafc"),   # IMAFC核心
            ("n300fd", "rv32imafdc"), # IMAFDC核心
            ("n307fd", "rv32imafdc"), # IMAFDC核心
            # 600系列测试
            ("n600", "rv32imac"),     # RV32 IMAC核心
            ("nx600", "rv64imac"),    # RV64 IMAC核心
            ("u600fd", "rv32imafdc"),# RV32 IMAFDC核心
            ("nx600fd", "rv64imafdc"),# RV64 IMAFDC核心
            # 900和1000系列
            ("n900", "rv32imac"),     # RV32 IMAC核心
            ("nx900fd", "rv64imafdc"),# RV64 IMAFDC核心
            ("nx1000fd", "rv64imafdc")# RV64 IMAFDC核心
        ]

        for core, arch in test_cases:
            os.environ["SDK_SUPPORT_ARCH"] = arch
            config = {
            "build_config": {
                "CORE": core,
                "ARCH_EXT": ""
            }
            }
            filtered, msg = filter_app_config(config)
            self.assertEqual(filtered, False,
            f"Failed for core {core} with arch {arch}")

    def test_advanced_extension_combinations(self):
        """测试高级扩展组合"""
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdcv_zve64d_zkn_zks"

        test_cases = [
            # Vector扩展组合
            ("v_zve64d_zvl128b", False),         # 基本向量组合
            ("v_zve64d_zvl256b", True),          # 不支持的向量长度
            ("v_zve32x_zve64x_zve64d", False),   # 向量扩展完整组合

            # K扩展组合
            ("k_zkn_zkr_zkt_zks", True),        # K扩展完整组合
            ("_zkn", False),                     # K扩展支持组合
            ("k_zkne_zknd_zknh", True),         # K加密扩展组合
            ("k_zbkb_zbkc_zbkx", True),         # K位操作扩展组合

            # 混合扩展
            ("_zvk_zve64d_zkn", True),           # 向量+K扩展
            ("_zvk_zve64d_zkn_zicsr", True),     # 向量+K+基础扩展
            ("_zvk_zve64d_invalid", True),        # 包含无效扩展

            # 特殊组合
            ("_zicsr_zifencei_zc", True),        # 基础扩展组合
            ("_zicsr_zifencei", False),          # 带前导下划线
            ("_zc_zicsr_zifencei", True),        # 压缩指令组合
        ]

        for ext, should_filter in test_cases:
            config = {
                "build_config": {
                    "CORE": "nx1000fd",
                    "ARCH_EXT": ext
                }
            }
            filtered, msg = filter_app_config(config)
            self.assertEqual(filtered, should_filter, f"Failed for extension {ext}")
            if should_filter and "invalid" in ext:
                self.assertTrue("not supported" in msg)

    def test_dsp_advanced(self):
        """测试DSP扩展的高级组合"""
        test_cases = [
            # RV32 DSP测试
            ("rv32imacp_xxldsp", "p", False),
            ("rv32imacp_xxldspn1x", "p_dspn1", False),
            ("rv32imacp_xxldspn2x", "p_dspn2", False),
            ("rv32imacp", "p_dspn3", True),

            # RV64 DSP测试
            ("rv64imacp_xxldsp", "p", False),
            ("rv64imacp_xxldspn3x", "p_dspn3", False),

            # DSP组合测试
            ("rv32imacp_xxldspn3x", "p_dsp_dspn1_dspn2_dspn3", False),
            ("rv32imacp_xxldsp", "p_dspn1_dspn2", True),
        ]

        for arch, ext, should_filter in test_cases:
            os.environ["SDK_SUPPORT_ARCH"] = arch
            config = {
            "build_config": {
                "CORE": "n300" if arch.startswith("rv32") else "nx900",
                "ARCH_EXT": ext
            }
            }
            filtered, msg = filter_app_config(config)
            self.assertEqual(filtered, should_filter,
            f"Failed for {arch} with extension {ext}")

    def test_error_conditions(self):
        """测试错误情况处理"""
        os.environ["SDK_SUPPORT_ARCH"] = "rv64imafdcvk"

        test_cases = [
            # 架构不匹配
            ("nx1000fd", "v_invalid", True),     # 无效向量扩展
            ("n300", "k_invalid", True),         # RV32核心使用RV64扩展
            ("nx900fd", "p_invalid", True),      # 无效P扩展

            # 特殊字符处理
            ("nx1000fd", "vk", False),        # 多余的下划线
            ("nx1000fd", "v__k", True),         # 连续下划线
            ("nx1000fd", " v_k ", True),        # 额外空格

            # 极端情况
            ("nx1000fd", "", False),             # 空扩展
            ("nx1000fd", "_", False),            # 仅下划线
            ("nx1000fd", "___", False),          # 多个下划线
        ]

        for core, ext, should_filter in test_cases:
            config = {
                "build_config": {
                    "CORE": core,
                    "ARCH_EXT": ext
                }
            }
            filtered, msg = filter_app_config(config)
            self.assertEqual(filtered, should_filter,
                f"Failed for core {core} with extension {ext}")
            if should_filter and "invalid" in ext:
                self.assertTrue("not supported" in msg)

if __name__ == '__main__':
    unittest.main()

