package foundation.helper;

import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.text.DecimalFormat;

import foundation.activeandroid.util.Log;

public class FilesUtils {
    public static long getFileSizes(File f) throws Exception {// 取得文件大小
        long s = 0;
        if (f.exists()) {
            FileInputStream fis = null;
            fis = new FileInputStream(f);
            s = fis.available();
        } else {
            f.createNewFile();
            System.out.println("文件不存在");
        }
        return s;
    }

    // 递归
    public static long getFileSize(File f) throws Exception {
        long size = 0;
        File flist[] = f.listFiles();
        for (int i = 0; i < flist.length; i++) {
            if (flist[i].isDirectory()) {
                size = size + getFileSize(flist[i]);
            } else {
                size = size + flist[i].length();
            }
        }
        return size;
    }

    public static String FormetFileSize(long fileS) {// 转换文件大小
        DecimalFormat df = new DecimalFormat("#.00");
        String fileSizeString = "";
        if (fileS > 0) {
            if (fileS < 1024) {
                fileSizeString = df.format((double) fileS) + "B";
            } else if (fileS < 1048576) {
                fileSizeString = df.format((double) fileS / 1024) + "KB";
            } else if (fileS < 1073741824) {
                fileSizeString = df.format((double) fileS / 1048576) + "MB";
            } else {
                fileSizeString = df.format((double) fileS / 1073741824) + "G";
            }
        } else {
            fileSizeString = "0.0B";
        }
        return fileSizeString;
    }


    public static String formetFileSize(long fileS) {// 转换文件大小
        String fileSizeString = "";
        if (fileS > 0) {
            if (fileS < 1024) {
                fileSizeString = fileS + "B";
            } else if (fileS < 1048576) {
                fileSizeString = (int) (fileS / 1024) + "KB";
            } else if (fileS < 1073741824) {
                fileSizeString = (int) (fileS / 1048576) + "MB";
            } else {
                fileSizeString = (int) (fileS / 1073741824) + "G";
            }
        } else {
            fileSizeString = "0B";
        }
        return fileSizeString;
    }

    public static long getlist(File f) {// 递归求取目录文件个数
        long size = 0;
        File flist[] = f.listFiles();
        size = flist.length;
        for (int i = 0; i < flist.length; i++) {
            if (flist[i].isDirectory()) {
                size = size + getlist(flist[i]);
                size--;
            }
        }
        return size;
    }

    public static void delete(File file) {
        File flist[] = file.listFiles();
        for (int i = 0; i < flist.length; i++) {
            if (flist[i].isDirectory()) {
                delete(flist[i]);
            } else {
                flist[i].delete();
            }
        }
    }

    // 转换文件大小
    public static String formatFileSize(long size) {
        double fileSize;
        String fileSizeString;
        if (size < 1048576) {
            fileSize = ((double) size / 1024);
            fileSizeString = String.format("%.2fKB", fileSize);
        } else if (size < 1073741824) {
            fileSize = ((double) size / 1048576);
            fileSizeString = String.format("%.2fMB", fileSize);
        } else {
            fileSize = ((double) size / 1073741824);
            fileSizeString = String.format("%.2fGB", fileSize);
        }
        return fileSizeString;
    }

    public static void deleteFile(File file) {
        if (file.exists()) { // 判断文件是否存在
            if (file.isFile()) { // 判断是否是文件
                file.delete(); // delete()方法 你应该知道 是删除的意思;
            } else if (file.isDirectory()) { // 否则如果它是一个目录
                File files[] = file.listFiles(); // 声明目录下所有的文件 files[];
                for (int i = 0; i < files.length; i++) { // 遍历目录下所有的文件
                    deleteFile(files[i]); // 把每个文件 用这个方法进行迭代
                }
            }
            file.delete();
        }
    }

    /**
     * 文件拷贝
     *
     * @param srcFileName  源文件
     * @param destFileName 目标路径
     * @return
     */
//    public static boolean copyFile(String srcFileName, String destFileName) {
//        File srcFile = new File(srcFileName);
//        // 判断目标文件是否存在
//        if(!srcFile.exists()) {
//            return false;
//        }
//
//        File destFile = new File(destFileName);
//        // 复制文件
//        int byteread = 0; // 读取的字节数
//        FileInputStream in = null;
//        FileOutputStream out = null;
//        try {
//            in = new FileInputStream(srcFile);
//            out = new FileOutputStream(destFile);
//            byte[] buffer = new byte[4096];
//
//            while ((byteread = in.read(buffer)) != -1) {
//                out.write(buffer, 0, byteread);
//            }
//            return true;
//        } catch (FileNotFoundException e) {
//            return false;
//        } catch (IOException e) {
//            return false;
//        } finally {
//            close(in);
//            close(out);
//        }
//    }
    public static boolean copyFile(String srcFileName, String destFileName) {
        File srcFile = new File(srcFileName);
        // 判断目标文件是否存在
        if (!srcFile.exists()) {
            return false;
        }

        File destFile = new File(destFileName);
        // 复制文件
        FileInputStream in = null;
        FileOutputStream out = null;
        FileChannel channelIn = null;
        FileChannel channelOut = null;
        long start = System.currentTimeMillis();
        try {
            in = new FileInputStream(srcFile);
            out = new FileOutputStream(destFile);
            channelIn = in.getChannel();
            channelOut = out.getChannel();
            long size = channelIn.size();
            ByteBuffer buffer = ByteBuffer.allocate(32 * 1024);
            for (; ; ) {
                int readSize = channelIn.read(buffer);
                buffer.flip();
                if (readSize > 0) {
                    channelOut.write(buffer);
                    buffer.clear();
                } else {
                    break;
                }
            }
            Log.e("copyFile", String.format("size->%s, time->%s", size, System.currentTimeMillis() - start));
            return true;
        } catch (FileNotFoundException e) {
            return false;
        } catch (IOException e) {
            return false;
        } finally {
            close(in);
            close(out);
            close(channelIn);
            close(channelOut);
        }
    }

    public static void close(Closeable closeable) {
        if (closeable != null) {
            try {
                closeable.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void ensureExist(File file){
        if (file == null) return;
        if (!file.exists()){
            file.mkdirs();
        }
    }

}
