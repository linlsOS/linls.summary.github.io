# Vibrator cts

Vibrator cts测试方法

# testVibrator

```java
// cts/tests/tests/permission/src/android/permission/cts/NoSystemFunctionPermissionTest.java

/**
 * Verify that Vibrator's vibrating related methods requires permissions.
 * <p>Requires Permission:
 *   {@link android.Manifest.permission#VIBRATE}.
 */
@SmallTest
public void testVibrator() {
    Vibrator vibrator = (Vibrator)getContext().getSystemService(Context.VIBRATOR_SERVICE);

    try {
        vibrator.cancel();
        fail("Vibrator.cancel() did not throw SecurityException as expected.");
    } catch (SecurityException e) {
        // expected
    }

    try {
        vibrator.vibrate(1);
        fail("Vibrator.vibrate(long) did not throw SecurityException as expected.");
    } catch (SecurityException e) {
        // expected
    }

    long[] testPattern = {1, 1, 1, 1, 1};

    try {
        vibrator.vibrate(testPattern, 1);
        fail("Vibrator.vibrate(long[], int) not throw SecurityException as expected.");
    } catch (SecurityException e) {
        // expected
    }
}
```
