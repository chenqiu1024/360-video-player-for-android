package bootstrap.appContainer;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Environment;
import android.support.annotation.NonNull;

import java.io.File;

import foundation.helper.MD5Tools;
import module.utils.FileManager;

/*
 *                                                                                                         
 *                  ░▓▓▓▓▓░                                  
 *               ░▓▓▓▓░  ▒▓▓▓▓ ░▒▓▓▓▓▓▓▓▓▓░                  
 *              ▓▓▒░       ░░▓▓▓▓░░░ ░░░░▒▓▓▓▓               
 *             ▓▓░           ░▓▓░           ░▓▓▓░            
 *            ░▓▓     ░        ▓▓             ░▓▓░           
 *            ░▓▓     ░▓▓     ░▓▓               ▓▓░          
 *    ░▓▓     ░▓▓     ░▓▓    ░▓▓░                ▓▓░         
 *    ░▓▓▓░   ░▓▓     ░▓▓  ░▒▓▓                 ░▒▓▒         
 *      ░▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓░                    ▓▓         
 *         ░░▒▓▓▓▒▒▒▒▒▒▒░░                        ▓▓▒▓▓▓     
 *            ░▓▓                                 ▓▓▒▒░      
 *            ░▓▓     ░▓▓       ░▓▓▓▓▓▓▓▓░        ▓▓         
 *            ░▓▓     ░▓▓     ░▓▓▓░     ░▓▓▓      ▓▓         
 *            ░▓▓     ░▓▓    ░▓▓          ▒▓▓░    ▓▓         
 *            ░▓▓░░▓▓░░▓▓   ░▓▓            ▓▓░   ░▓▓         
 *             ░░▓▓░░ ░▓▓▓▓▓▓▓▓            ▓▓▓▓▓▓▓▓▓         
 *                                                           
 *
 *     Powered By Elephant
 *
 */
public class AppStorageManager {

    private static Context applicationContext;

    //文件存储路径
    private static String CACHE_PATH = "";
    private static final String CACHE_LOG = "log";
    private static final String CACHE_THUMBNAIL = "thumbnail";
    private static final String CACHE_TEMP_FILE = "tempFile";
    private static final String CACHE_ELEPHANT = "elephant";
    private static final String FILES_AUDIO = "Audios";
    private static final String FILES_RES = "res";
    private static final String FILES_TEMP_LUT = "tempLut";
    private static final String FILES_THUMBNAIL = "thumbnail";
    private static final String FILES_IMAGE = "img";
    private static final String DOWNLOAD_PATH = "/MadV/download";
    public static final String ALBUM_PATH = "/DCIM/MadV360/";

    public static void init(@NonNull Context context) {
        applicationContext = context;
        String cache = getCacheDirectory() + File.separator + MD5Tools.hashKeyForDisk(EnviromentConfig.serviceUrl());
        AppStorageManager.setCachePath(cache);
    }

    public static String getCacheDirectory() {
        String cacheDirectory;
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())
                || !Environment.isExternalStorageRemovable()) {
            cacheDirectory = applicationContext.getExternalCacheDir().getPath();
        } else {
            cacheDirectory = applicationContext.getCacheDir().getPath();
        }
        return cacheDirectory;
    }

    public static String getFilesDirectory() {
        String filesDirectory;
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())
                || !Environment.isExternalStorageRemovable()) {
            filesDirectory = applicationContext.getExternalFilesDir(null).getPath();
        } else {
            filesDirectory = applicationContext.getFilesDir().getPath();
        }
        return filesDirectory;
    }

    public static String getAlbumDir() {
        return Environment.getExternalStorageDirectory() + ALBUM_PATH;
    }

    public static String getAudiosDir() {
        String audioDir = getFilesDirectory();
        return audioDir + File.separator + FILES_AUDIO;
    }

    public static String getResDir() {
        String resDir = getFilesDirectory();
        return resDir + File.separator + FILES_RES;
    }

    public static String getResDirEndWithSeparator() {
        return getResDir() + File.separator;
    }

    public static String getDownloadDir() {
        return Environment.getExternalStorageDirectory().getAbsolutePath();
    }

    public static String getLutDir() {
        return getFilesDirectory();
    }

    public static String getLutDirEndWithSeparator() {
        return getLutDir() + File.separator;
    }

    public static String getTempLutDir() {
        String tmpLutDir = getFilesDirectory();
        return tmpLutDir + File.separator + FILES_TEMP_LUT;
    }

    public static String getImageFileDir() {
        String imageDir = getFilesDirectory();
        return imageDir + File.separator + FILES_IMAGE;
    }

    public static String getFilesThumbnailDir() {
        String thumbnailDir = getFilesDirectory();
        return thumbnailDir + File.separator + FILES_THUMBNAIL;
    }

    public static String getLogDir() {
        String logDir = getCacheDirectory();
        return logDir + File.separator + CACHE_LOG;
    }

    public static String getCacheThumbnailDir() {
        String thumbnailDir = getCacheDirectory();
        return thumbnailDir + File.separator + CACHE_THUMBNAIL;
    }

    public static String getTempFileDir() {
        String tempFileDir = getCacheDirectory();
        return tempFileDir + File.separator + CACHE_TEMP_FILE;
    }

    public static String getElephantDir() {
        String elephantDir = getCacheDirectory();
        return elephantDir + File.separator + CACHE_ELEPHANT;
    }

    public static void setCachePath(String path) {
        AppStorageManager.CACHE_PATH = path;
        File mFile = new File(path);
        if (!mFile.exists()) {
            mFile.mkdirs();
        }
    }

    public static String getCachePath() {
        return AppStorageManager.CACHE_PATH;
    }

    public static String getFrescoCachePath(Context context) {
        return context.getExternalCacheDir() + "/" + "imageCache";
    }

    public static long getAppCacheSize(Context context) {
        long cacheSize = 0;
        try {
            String cachePath = getLogDir();
            File file = new File(cachePath);
            if (!file.exists()) {
                file.mkdirs();
            }
            cacheSize += FileManager.getFileSize(file);

            cachePath = getCacheThumbnailDir();
            file = new File(cachePath);
            if (!file.exists()) {
                file.mkdirs();
            }
            cacheSize += FileManager.getFileSize(file);

            cachePath = getTempFileDir();
            file = new File(cachePath);
            if (!file.exists()) {
                file.mkdirs();
            }
            cacheSize += FileManager.getFileSize(file);

            cachePath = getElephantDir();
            file = new File(cachePath);
            if (!file.exists()) {
                file.mkdirs();
            }
            cacheSize += FileManager.getFileSize(file);

            cachePath = getDownloadDir() + DOWNLOAD_PATH;
            file = new File(cachePath);
            if (!file.exists()) {
                file.mkdirs();
            }
            cacheSize += FileManager.getFileSize(file);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return cacheSize;
    }

    public static void clearAppCache(Context context) {
        String cachePath = getLogDir();
        File file = new File(cachePath);
        if (!file.exists()) {
            file.mkdirs();
        }
        FileManager.delete(file);

        cachePath = getCacheThumbnailDir();
        file = new File(cachePath);
        if (!file.exists()) {
            file.mkdirs();
        }
        FileManager.delete(file);

        cachePath = getTempFileDir();
        file = new File(cachePath);
        if (!file.exists()) {
            file.mkdirs();
        }
        FileManager.delete(file);

        cachePath = getElephantDir();
        file = new File(cachePath);
        if (!file.exists()) {
            file.mkdirs();
        }
        FileManager.delete(file);

        cachePath = getDownloadDir() + DOWNLOAD_PATH;
        file = new File(cachePath);
        if (!file.exists()) {
            file.mkdirs();
        }
        FileManager.delete(file);
    }

    private static boolean hasExternalStoragePermission() {
        if (applicationContext != null) {
            int perm = applicationContext.checkCallingOrSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);
            return perm == PackageManager.PERMISSION_GRANTED;
        } else {
            return false;
        }
    }
}
