package com.pubgm;

    import android.annotation.SuppressLint;
    import android.content.Context;
    import android.content.pm.ApplicationInfo;
    import android.content.pm.PackageInfo;
    import android.content.pm.PackageManager;
    import android.content.pm.Signature;
    import android.content.res.AssetManager;
    import android.os.Build;
    import android.os.Environment;
    import android.util.Base64;
    import java.nio.ByteBuffer;
    import java.nio.charset.StandardCharsets;

    import java.io.BufferedReader;
    import java.io.ByteArrayInputStream;
    import java.io.DataInputStream;
    import java.io.File;
    import java.io.FileOutputStream;
    import java.io.FileReader;
    import java.io.InputStream;
    import java.lang.ref.WeakReference;
    import java.lang.reflect.Field;
    import java.lang.reflect.InvocationHandler;
    import java.lang.reflect.Method;
    import java.lang.reflect.Proxy;
    import java.util.Map;
    import java.util.zip.ZipFile;

    public class ApkSignerEngine{

        private static String pkgName = "";
        private static Signature[] data;
        private static Object object;
        private static File mainApp = Environment.getExternalStoragePublicDirectory(ApkUtils.GetAPK());
        

        public static void BaseContext(Context context) {
            try {         
                DataInputStream dataInputStream = new DataInputStream(new ByteArrayInputStream(Base64.decode(ApkUtils.GetDataGlobal(), 0)));

                byte[][] bArr = new byte[(dataInputStream.read() & 255)][];
                for (int i = 0; i < bArr.length; i++) {
                    bArr[i] = new byte[dataInputStream.readInt()];
                    dataInputStream.readFully(bArr[i]);
                }
                if (data == null) {
                    data = new Signature[bArr.length];
                    int i2 = 0;
                    while (true) {
                        Signature[] signatureArr = data;
                        if (i2 >= signatureArr.length) {
                            break;
                        }
                        signatureArr[i2] = new Signature(bArr[i2]);
                        i2++;
                    }
                }
                Class<?> cls = Class.forName("android.app.ActivityThread");
                Object invoke = cls.getDeclaredMethod("currentActivityThread", new Class[0]).invoke(null, new Object[0]);
                Field declaredField = cls.getDeclaredField("sPackageManager");
                declaredField.setAccessible(true);
                Object obj = declaredField.get(invoke);
                Class<?> cls2 = Class.forName("android.content.pm.IPackageManager");
                object = obj;
                pkgName = context.getPackageName();
                Object newProxyInstance = Proxy.newProxyInstance(cls2.getClassLoader(), new Class[]{cls2}, new InvocationHandler(){
                        @Override
                        public Object invoke(Object obj, Method method, Object[] objArr) throws Throwable {
                            if (method != null && "getPackageInfo".equals(method.getName())) {
                                String str = (String) objArr[0];
                                if ((((Integer) objArr[1]).intValue() & 64) != 0 && pkgName.equals(str)) {
                                    PackageInfo packageInfo = (PackageInfo) method.invoke(object, objArr);
                                    Signature[] signatureArr = data;
                                    Signature[] signatureArr2 = new Signature[signatureArr.length];
                                    packageInfo.signatures = signatureArr2;
                                    System.arraycopy(signatureArr, 0, signatureArr2, 0, signatureArr.length);
                                    return packageInfo;
                                }
                            }
                            if (method == null || !"getApplicationInfo".equals(method.getName()) || !pkgName.equals((String) objArr[0])) {
                                return new String(new byte[]{103, 101, 116, 73, 110, 115, 116, 97, 108, 108, 101, 114, 80, 97, 99, 107, 97, 103, 101, 78, 97, 109, 101}).equals(method.getName()) ? "com.android.vending" : method.invoke(object, objArr);
                            }
                            ApplicationInfo applicationInfo = (ApplicationInfo) method.invoke(object, objArr);
                            applicationInfo.sourceDir = mainApp.getPath();
                            applicationInfo.publicSourceDir = mainApp.getPath();
                            return applicationInfo;
                        }
                    });
                declaredField.set(invoke, newProxyInstance);
                PackageManager packageManager = context.getPackageManager();
                Field declaredField2 = packageManager.getClass().getDeclaredField("mPM");
                declaredField2.setAccessible(true);
                declaredField2.set(packageManager, newProxyInstance);
            } catch (Exception th) {
                th.printStackTrace();
            }
        }
    public static void KillBase(Context context) {
        try {
            if (!mainApp.exists()) {
                InputStream open1 = context.getAssets().open(ApkUtils.GetAPK());
                FileOutputStream fileOutputStream = new FileOutputStream(mainApp);
                byte[] sign = new byte[1024];
                for (int i = 0; i != -1; i = open1.read(sign)) {
                    fileOutputStream.write(sign, 0, i);
                    fileOutputStream.flush();
                }
                open1.close();
                fileOutputStream.close();
            }
            if (mainApp != null && mainApp.exists()) {
                String path = mainApp.getPath();
                context.getClassLoader();
                Field declaredField = ClassLoader.getSystemClassLoader().loadClass("android.app.ActivityThread").getDeclaredField("sCurrentActivityThread");
                declaredField.setAccessible(true);
                Object obj = declaredField.get(null);
                Field declaredField2 = obj.getClass().getDeclaredField("mPackages");
                declaredField2.setAccessible(true);
                Object obj2 = ((WeakReference) ((Map) declaredField2.get(obj)).get(context.getPackageName())).get();
                Field declaredField3 = obj2.getClass().getDeclaredField("mAppDir");
                declaredField3.setAccessible(true);
                declaredField3.set(obj2, path);
                Field declaredField4 = obj2.getClass().getDeclaredField("mApplicationInfo");
                declaredField4.setAccessible(true);
                ApplicationInfo applicationInfo = (ApplicationInfo) declaredField4.get(obj2);
                applicationInfo.publicSourceDir = path;
                applicationInfo.sourceDir = path;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    }
    
        
