package bootstrap.appContainer;

/*
 *	 ______    ______    ______
 *	/\  __ \  /\  ___\  /\  ___\
 *	\ \  __<  \ \  __\_ \ \  __\_
 *	 \ \_____\ \ \_____\ \ \_____\
 *	  \/_____/  \/_____/  \/_____/
 *
 *
 *	Copyright (c) 2013-2014, {Bee} open source community
 *	http://www.bee-framework.com
 *
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a
 *	copy of this software and associated documentation files (the "Software"),
 *	to deal in the Software without restriction, including without limitation
 *	the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *	and/or sell copies of the Software, and to permit persons to whom the
 *	Software is furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *	IN THE SOFTWARE.
 */

import android.content.Context;
//import android.support.multidex.MultiDex;
//
//import com.esotericsoftware.kryo.io.ByteBufferInputStream;
//import com.facebook.FacebookSdk;
//import com.facebook.drawee.backends.pipeline.Fresco;
//import com.madv360.madv.common.MVThumbnailDiskCache;
//import com.madv360.madv.utils.CrashHandlerUtil;
//import com.madv360.madv.utils.MediaScannerUtil;
//import com.tencent.bugly.crashreport.CrashReport;
//import com.umeng.analytics.MobclickAgent;

import java.io.DataInput;
import java.io.DataInputStream;
import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

//import app.AppConst;
//import cn.jpush.android.api.JPushInterface;
import foundation.activeandroid.ActiveAndroid;
import foundation.activeandroid.app.Application;
import foundation.downloader.bizs.DLManager;
//import foundation.log.Config;
//import foundation.log.MvLog;
//import foundation.log.bugly.BuglyLogger;
//import foundation.log.bugly.BuglyUploader;
//import module.imagepicker.utils.MVBitmapCache;
//import plugin.debug.ElephantDebuger;

public class ElephantApp extends Application {
    private static ElephantApp instance;

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
//        MultiDex.install(this);//依赖分包处理
    }

    public static ElephantApp getInstance() {
        if (instance == null) {
            instance = new ElephantApp();
        }
        return instance;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;

//        JPushInterface.init(this);            // 初始化 JPush
//        initConfig();
//
//        MVBitmapCache.init(this);
//        MVThumbnailDiskCache.init(this);
//        initCrashHandler();
//
//        if (EnviromentConfig.environment() == EnviromentConfig.ENVIRONMENT_DEVELOPMENT
//                || EnviromentConfig.environment() == EnviromentConfig.ENVIRONMENT_MOCKSERVER) {
//            new ElephantDebuger().showBug(this);
//        }

        File albumDirectory = new File(AppStorageManager.getAlbumDir());
        if (!albumDirectory.exists()) {
            albumDirectory.mkdirs();
        }
//        MobclickAgent.setCatchUncaughtExceptions(false);        //取消友盟上传崩溃日志
//        CrashReport.initCrashReport(getApplicationContext(), UserAppConst.SDK_BUGLY_APP_ID, AppConst.DEBUG);
//
//        Config config = new Config.Builder()
//                .logger(new BuglyLogger())
//                .uploader(new BuglyUploader())
//                .debug(AppConst.DEBUG)
//                .build();
//        MvLog.config(config);
//        //去扫描一下已经下载过的内容，生成缩略图
//        MediaScannerUtil.scanAlbumDir();
    }

//    protected void initCrashHandler() {
//        CrashHandlerUtil.getInstance().init(this);
//    }

    protected void initConfig() {
//        //初始化nosql数据库
//        FileCacheImpl.init(this);
        //初始化存储管理
        AppStorageManager.init(this);
//        //初始化APP信息
//        SESSION.init(this);
//        //初始化App网络请求错误处理
//        AppNetErrorHandler.init(this);
//        //Facebook
//        FacebookSdk.sdkInitialize(this);
//        //初始化文件下载库
//        DLManager.initialize(this, AppStorageManager.getAudiosDir());
//        //初始化Fresco
//        Fresco.initialize(this, ImagePipelineConfigFactory.getImagePipelineConfig(this));
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        ActiveAndroid.dispose();
    }
}
