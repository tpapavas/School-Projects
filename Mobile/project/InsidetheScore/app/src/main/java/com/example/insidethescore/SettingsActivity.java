package com.example.insidethescore;

import android.content.Intent;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.PopupMenu;
import android.widget.Switch;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import java.util.Locale;

public class SettingsActivity extends AppCompatActivity
        implements PopupMenu.OnMenuItemClickListener {

    Button selectLanguageButton;
    Switch enableDarkModeSwitch;

    private static String countryCode;

    private static boolean isNightModeOn = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_settings);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        selectLanguageButton = (Button) findViewById(R.id.select_language_button);
        enableDarkModeSwitch = (Switch) findViewById(R.id.enable_dark_mode_switch);

        setLangButtonText();

        enableDarkModeSwitch.setChecked(isNightModeOn);
        enableDarkModeSwitch.setText(isNightModeOn ? "dark" : "light");
    }

    public void showPopup(View view) {
        PopupMenu popup = new PopupMenu(this,view);
        popup.setOnMenuItemClickListener(this);
        popup.inflate(R.menu.language_options_menu);
        popup.show();
    }

    @Override
    public boolean onMenuItemClick(MenuItem item) {
        if (item.getItemId() == R.id.greek_language_option) {
            setLocale("el");
            setCountryCode("el");
        } else if (item.getItemId() == R.id.english_language_option) {
            setLocale("en");
            setCountryCode("en");
            selectLanguageButton.setText(getString(R.string.english_language_textview));
        }

        setLangButtonText();

        return true;
    }

    /**
     * Sets the locale of the app.
     * Code taken from https://stackoverflow.com/questions/12908289/how-to-change-language-of-app-when-user-selects-language
     * @param lang
     */
    public void setLocale(String lang) {
        Locale myLocale = new Locale(lang);
        Resources res = getResources();
        DisplayMetrics dm = res.getDisplayMetrics();
        Configuration conf = res.getConfiguration();
        conf.setLocale(myLocale);
        res.updateConfiguration(conf, dm);
//        createConfigurationContext(conf);

        Intent refresh = new Intent(this, SettingsActivity.class);
        finish();
        startActivity(refresh);
    }

    public void setLightDarkMode(View view) {
        if (!isNightModeOn) {
            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_YES);
            isNightModeOn = true;
        } else {
            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO);
            isNightModeOn = false;
        }

        Log.d("setLightDarkMode", "countryCode memory address " + System.identityHashCode(countryCode));
        Log.d("setLightDarkMode", "countryCode " + countryCode);
    }

    @Override
    protected void onNightModeChanged(int mode) {
        super.onNightModeChanged(mode);

        setLocale(countryCode);
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        setLocale(countryCode);
    }

    public static void setIsNightModeOn(boolean nightModeOn) {
        isNightModeOn = nightModeOn;
    }

    public static void setCountryCode(String countryCode1) {
        countryCode = countryCode1;
    }


    private void setLangButtonText() {
        switch (countryCode) {
            case "el":
                selectLanguageButton.setText(getString(R.string.greek_language_textview));
                break;
            case "en":
                selectLanguageButton.setText(getString(R.string.english_language_textview));
                break;
            default:
                selectLanguageButton.setText(getString(R.string.select_language_button_text));
        }
    }
}