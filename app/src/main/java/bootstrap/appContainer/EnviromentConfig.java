package bootstrap.appContainer;

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
public class EnviromentConfig {

    public static final int ENVIRONMENT_PRODUCTION = 1;
    public static final int ENVIRONMENT_DEVELOPMENT = 2;
    public static final int ENVIRONMENT_MOCKSERVER = 3;

    public static String mPreviewServelUrl;

    public static int environment() {
        return ENVIRONMENT_PRODUCTION;
    }

    public static String serviceUrl() {
        if (null != getPreviewServerUrl()) {
            return getPreviewServerUrl();
        }

        if (ENVIRONMENT_PRODUCTION == EnviromentConfig.environment()) {
            return "http://api.madv360.com:9999";
        } else {
            return "http://api.madv360.com:9999";
        }
    }

    public static String hostUrl() {
        if (ENVIRONMENT_PRODUCTION == EnviromentConfig.environment()) {
            return "http://XXX.XXX.XXX/api";
        } else {
            return "http://XXX.XXX.XXX/api";
        }
    }

    public static void setPreviewServerUrl(String serverUrl) {
        EnviromentConfig.mPreviewServelUrl = serverUrl;
    }

    public static void CleanPreviewServerUrl() {
        EnviromentConfig.mPreviewServelUrl = null;
    }

    public static String getPreviewServerUrl() {
        return EnviromentConfig.mPreviewServelUrl;
    }
}
