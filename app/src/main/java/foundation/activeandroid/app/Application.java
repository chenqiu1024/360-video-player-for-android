package foundation.activeandroid.app;

/*
 * Copyright (C) 2010 Michael Pardo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import android.content.SharedPreferences;
import foundation.activeandroid.ActiveAndroid;
import foundation.activeandroid.DatabaseHelper;

public class Application extends android.app.Application {

	public SharedPreferences shared;
	public SharedPreferences.Editor editor;
	@Override
	public void onCreate() {
		super.onCreate();
		shared = getSharedPreferences("DB", 0);
		editor = shared.edit();

		Integer aaVersion = DatabaseHelper.getDbVersion(this);
		if(aaVersion > shared.getInt("versioncode", 0)) {
			this.deleteDatabase(DatabaseHelper.getDbName(this));
			editor.putInt("versioncode", aaVersion);
			editor.apply();
		}

		ActiveAndroid.initialize(this);
	}
	
	@Override
	public void onTerminate() {
		super.onTerminate();
		ActiveAndroid.dispose();
	}
}