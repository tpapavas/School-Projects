package com.example.insidethescore;

import android.content.Intent;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.TableLayout;

import androidx.activity.EdgeToEdge;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.fragment.app.FragmentContainerView;
import androidx.recyclerview.widget.RecyclerView;

import com.example.insidethescore.auxiliary.ScoreDatabaseManager;
import com.example.insidethescore.music.MusicWork;

import java.util.Set;

public class MainActivity extends AppCompatActivity {
    RecyclerView recyclerView;
    RecyclerView.LayoutManager layoutManager;
    RecyclerView.Adapter<RecyclerAdapter.ViewHolder> adapter;

    FragmentContainerView navBar;

    MusicWork mw;

    ScoreDatabaseManager scoreDBManager;
    ActivityResultLauncher<Intent> launcher;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        // this is used instead of intents.
        launcher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result -> {
                    if (result.getResultCode() == RESULT_OK) {
                        Intent data = result.getData();
//                    tvResult.setText(data.getStringExtra("result"));
                    }
                }
        );

        scoreDBManager = new ScoreDatabaseManager();

        // check what mode (light/dark) is currently on
        if (AppCompatDelegate.getDefaultNightMode() == AppCompatDelegate.MODE_NIGHT_NO) {
            SettingsActivity.setIsNightModeOn(false);
        } else if (AppCompatDelegate.getDefaultNightMode() == AppCompatDelegate.MODE_NIGHT_YES) {
            Log.d("Settings onCreate", "isNightMode on: " + true);
            SettingsActivity.setIsNightModeOn(true);
        } else {
            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO);
            SettingsActivity.setIsNightModeOn(false);
        }

        // set the dimensions of main page cards dynamically
        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);

        int Height = displayMetrics.heightPixels;
        int Width = displayMetrics.widthPixels;

        TableLayout cards = (TableLayout) findViewById(R.id.cards);
        ViewGroup.LayoutParams layoutParams = cards.getLayoutParams();
        if (Width < Height) {
            layoutParams.height = (int) displayMetrics.widthPixels;
        } else {
            layoutParams.width = (int) displayMetrics.heightPixels;
        }
        cards.setLayoutParams(layoutParams);

        SettingsActivity.setCountryCode(getResources().getConfiguration().getLocales().get(0).getLanguage());
    }

    @Override
    protected void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
    }

    public void goToSearch(View view) {
        Intent intent = new Intent(this, SearchActivity.class);
        launcher.launch(intent);
    }

    public void goToSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        launcher.launch(intent);
    }

    public void goToLibrary(View view) {
        ImageButton libButton = (ImageButton) findViewById(R.id.library_button);
        libButton.performClick();
    }

    public void goToFavorites(View view) {
        ListPreviewActivity.setIsUpdated(true);
//        NavigationBar.getUserLists(false, this, launcher);

        // calls many other functions to retrieve music works of "Favorites" and open ListPreviewActivity page
        // to preview music works of "Favorites" list
        NavigationBar.getUserLists(false, this, launcher, ListPreviewActivity.class, null);
//        RecyclerAdapter.ViewHolder.getUserListMusicWorks("Favorites", this, ListPreviewActivity.class);
    }

    @Override
    public void onRestart() {
        super.onRestart();
//        finish();

        // perform home page refresh (for language change to take effect)
        ImageButton homeButton = (ImageButton) findViewById(R.id.home_button);
        homeButton.performClick();
    }
}