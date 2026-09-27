"""Evidence failures must fail the job; no external compiler/profiler needed."""
import contextlib
import importlib.util
import io
from pathlib import Path
import subprocess
import sys
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location("evidence", Path(__file__).resolve().parents[1] / "collapse_evidence.py")
evidence = importlib.util.module_from_spec(spec)
spec.loader.exec_module(evidence)


class EvidenceGates(unittest.TestCase):
    def test_broken_reference_is_not_success(self):
        with patch.object(sys, "argv", ["evidence"]), \
             patch.object(evidence, "available_builds", return_value={"gcc-O2": evidence.BUILDS["gcc-O2"]}), \
             patch.object(evidence, "discover_cases", return_value={"one_receiver": ["handwritten"]}), \
             patch.object(evidence, "measure", return_value={"error": "injected compile error"}), \
             patch.object(evidence, "run", return_value=subprocess.CompletedProcess([], 0, "test compiler\n", "")), \
             contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(evidence.main(), 1)

    def test_empty_selection_is_not_success(self):
        with patch.object(sys, "argv", ["evidence"]), \
             patch.object(evidence, "available_builds", return_value={}), \
             contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(evidence.main(), 2)

    def test_crash_after_checksum_is_not_success(self):
        with patch.object(evidence, "run", return_value=subprocess.CompletedProcess([], 1, "checksum 42\n", "failed")):
            checksum, error = evidence.checksum("unused")
            self.assertIsNone(checksum)
            self.assertIsNotNone(error)

    def test_profiler_failure_with_complete_dumps_is_not_success(self):
        def failed_profiler(command):
            output = next(arg.split("=", 1)[1] for arg in command if arg.startswith("--callgrind-out-file="))
            for phase in ("setup", "publish", "teardown"):
                Path(output.replace("%p", phase)).write_text(
                    f"desc: Trigger: Client Request: {phase}\nsummary: 1000\n")
            return subprocess.CompletedProcess(command, 1, "", "injected profiler failure")
        with patch.object(evidence, "run", side_effect=failed_profiler):
            _, error = evidence.callgrind("unused")
            self.assertIsNotNone(error)
            self.assertIn("exit=1", error)


if __name__ == "__main__":
    unittest.main()
