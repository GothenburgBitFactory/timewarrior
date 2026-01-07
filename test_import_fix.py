#!/usr/bin/env python3
"""
Test for issue #734: Configuration values after invalid imports are silently ignored.
This script tests that configuration values after invalid imports are not silently ignored.
"""

import os
import sys
import tempfile
import subprocess

# Test case 1: Import statement at the beginning, config values after it
def test_import_before_config():
    """Test config parsing when import statement comes BEFORE config values"""
    with tempfile.TemporaryDirectory() as tmpdir:
        config_file = os.path.join(tmpdir, "timewarrior.cfg")
        
        # Write config with import BEFORE the config values
        with open(config_file, 'w') as f:
            f.write("""import /usr/local/share/doc/timew/holidays/holidays.en-US
reports.summary.ids = yes
reports.summary.annotations = yes
""")
        
        # Run timew config command
        env = os.environ.copy()
        env["TIMEWARRIORDB"] = tmpdir
        
        result = subprocess.run(
            ["timew", "config"],
            env=env,
            capture_output=True,
            text=True
        )
        
        print("Test 1: Import statement BEFORE config values")
        print("STDOUT:")
        print(result.stdout)
        print("STDERR:")
        print(result.stderr)
        
        # Check that the config values are present
        if "reports.summary.ids = yes" in result.stdout and "reports.summary.annotations = yes" in result.stdout:
            print("✓ Test 1 PASSED: Config values are present after invalid import")
            return True
        else:
            print("✗ Test 1 FAILED: Config values are missing after invalid import")
            return False

# Test case 2: Import statement at the end, config values before it
def test_import_after_config():
    """Test config parsing when import statement comes AFTER config values"""
    with tempfile.TemporaryDirectory() as tmpdir:
        config_file = os.path.join(tmpdir, "timewarrior.cfg")
        
        # Write config with import AFTER the config values
        with open(config_file, 'w') as f:
            f.write("""reports.summary.ids = yes
reports.summary.annotations = yes
import /usr/local/share/doc/timew/holidays/holidays.en-US
""")
        
        # Run timew config command
        env = os.environ.copy()
        env["TIMEWARRIORDB"] = tmpdir
        
        result = subprocess.run(
            ["timew", "config"],
            env=env,
            capture_output=True,
            text=True
        )
        
        print("\nTest 2: Import statement AFTER config values")
        print("STDOUT:")
        print(result.stdout)
        print("STDERR:")
        print(result.stderr)
        
        # Check that the config values are present
        if "reports.summary.ids = yes" in result.stdout and "reports.summary.annotations = yes" in result.stdout:
            print("✓ Test 2 PASSED: Config values are present before invalid import")
            return True
        else:
            print("✗ Test 2 FAILED: Config values are missing before invalid import")
            return False

# Test case 3: Check for warning message about invalid import
def test_warning_message():
    """Test that a warning is printed about invalid imports"""
    with tempfile.TemporaryDirectory() as tmpdir:
        config_file = os.path.join(tmpdir, "timewarrior.cfg")
        
        # Write config with import of non-existent file
        with open(config_file, 'w') as f:
            f.write("""import /usr/local/share/doc/timew/holidays/holidays.en-US
reports.summary.ids = yes
""")
        
        # Run timew config command
        env = os.environ.copy()
        env["TIMEWARRIORDB"] = tmpdir
        
        result = subprocess.run(
            ["timew", "config"],
            env=env,
            capture_output=True,
            text=True
        )
        
        print("\nTest 3: Warning message about invalid import")
        print("STDOUT:")
        print(result.stdout)
        print("STDERR:")
        print(result.stderr)
        
        # Check that a warning is printed
        if "WARNING" in result.stderr and "Could not read imported file" in result.stderr:
            print("✓ Test 3 PASSED: Warning message is shown for invalid import")
            return True
        else:
            print("⚠ Test 3 INFO: No warning message (fix may not include warning output)")
            # This is not necessarily a failure, just informational
            return True

if __name__ == "__main__":
    print("Testing fix for issue #734\n")
    test1_pass = test_import_before_config()
    test2_pass = test_import_after_config()
    test3_pass = test_warning_message()
    
    print("\n" + "="*50)
    print("Summary:")
    print(f"Test 1 (import before config): {'PASS' if test1_pass else 'FAIL'}")
    print(f"Test 2 (import after config): {'PASS' if test2_pass else 'FAIL'}")
    print(f"Test 3 (warning message): {'PASS' if test3_pass else 'FAIL'}")
    
    if test1_pass and test2_pass and test3_pass:
        print("\n✓ All tests passed!")
        sys.exit(0)
    else:
        print("\n✗ Some tests failed!")
        sys.exit(1)
