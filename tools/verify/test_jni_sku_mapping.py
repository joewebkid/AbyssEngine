"""Keep the Android JNI SKU index mapping recovered from the original ARM binary."""

import re
import unittest
from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "src/platform/android/JniBridge.cpp").read_text()


class JniSkuMappingTests(unittest.TestCase):
    def test_original_arm_sku_order_and_default(self):
        expected = {
            "Consumable": (
                "iap_managed_credits_100_000",
                "iap_managed_credits_300_000",
                "iap_managed_credits_1_000_000",
                "iap_managed_credits_3_000_000",
                "iap_managed_10_000_000",
            ),
            "Premium": (
                "iap_managed_valkyrie",
                "iap_managed_kaamo_club",
                "iap_managed_supernova",
                "iap_managed_vip",
                "iap_managed_full_package",
            ),
        }
        for family, names in expected.items():
            with self.subTest(family=family):
                start = SOURCE.index(f"Java_net_fishlabs_googleplay_ToJNI_gof2hd2012get{family}SKU(")
                body = SOURCE[start:SOURCE.index("\n}", start)]
                cases = re.findall(r'case (\d+): return env->NewStringUTF\("([^"]+)"\);', body)
                self.assertEqual(cases, [(str(i), name) for i, name in enumerate(names)])
                self.assertIn("default: return nullptr;", body)


if __name__ == "__main__":
    unittest.main()
