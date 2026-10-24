"""Guard the Android store setter's ARM-backed UTF cleanup route."""

import unittest
from pathlib import Path


class JniItemInfoReleaseTests(unittest.TestCase):
    def test_setter_releases_through_original_helper(self):
        source = (Path(__file__).resolve().parents[2] / "src/platform/android/NdkHooks.cpp").read_text()
        setter = source.split("void ndk_setNativeItemInformationList(", 1)[1].split("\n#endif", 1)[0]
        self.assertIn("releaseStringUTFChars(env, elems[col][row], utf[col][row]);", setter)
        self.assertNotIn("env->ReleaseStringUTFChars(", setter)


if __name__ == "__main__":
    unittest.main()
