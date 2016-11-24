package bootstrap.appContainer;

public interface ConstantS {

	// 替换为开发者REDIRECT_URL
	public static final String SINA_REDIRECT_URL = "https://api.weibo.com/oauth2/default.html";

	// 微信登录URL
	public static final String WEIXIN_LOGIN_URL = "https://api.weixin.qq.com/sns/oauth2/access_token?";

	// 新支持scope：支持传入多个scope权限，用逗号分隔 
	public static final String SINA_SCOPE = "email,direct_messages_read,direct_messages_write,"
			+ "friendships_groups_read,friendships_groups_write,statuses_to_me_read,"
			+ "follow_app_official_microblog," + "invitation_write";

	public static final long XIAOMI_APPID = 2882303761517454273L;
	public static final String XIAOMI_REDIRET_URL = "http://api.madv360.com:9999/oauth-xm";
	public static final String XIAOMI_OPENID_URL = "https://open.account.xiaomi.com/user/openidV2";

	//分享Url的前面一部分
	public static final String SHARE_VIDEO_URL = "https://awsbj0-cdn.fds-ssl.api.xiaomi.com/webvr/index.html?";
	public static final String SHARE_IMAGE_URL = "https://awsbj0-cdn.fds-ssl.api.xiaomi.com/webvr/index.htm?";
	//用户协议
	public static final String USER_PROTOCOL_URL = "http://v.madv360.com:9999/html/about.html";


	public static final String WEIXIN_APP_ID = "wxd5322d965af2f7ed";
	public static final String WEIXIN_APP_SECRET = "b081f4396a695043cd716745bd2102d6";
	public static final String QQZone_API_ID = "1105400255";
	public static final String SINA_APP_KEY = "4080217621";

}
