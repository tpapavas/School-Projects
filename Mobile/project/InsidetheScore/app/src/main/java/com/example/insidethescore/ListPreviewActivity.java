package com.example.insidethescore;

import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.example.insidethescore.auxiliary.ScoreDatabaseManager;

public class ListPreviewActivity extends AppCompatActivity {
    RecyclerView recyclerView;
    RecyclerView.LayoutManager layoutManager;
    RecyclerView.Adapter<ListPreviewRecyclerAdapter.ViewHolder> adapter;

    TextView listTitleTextView;

    private static boolean isUpdated = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_list_preview);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        listTitleTextView = findViewById(R.id.list_preview_textview);
        listTitleTextView.setText(ScoreDatabaseManager.getCurrentList());

        recyclerView = findViewById(R.id.list_preview_recycler_view);
        //Set the layout of the items in the RecyclerView
        layoutManager = new LinearLayoutManager(this, RecyclerView.VERTICAL, false);
        recyclerView.setLayoutManager(layoutManager);
        //Set my Adapter for the RecyclerView
        adapter = new ListPreviewRecyclerAdapter();
        recyclerView.setAdapter(adapter);
    }

    /**
     * TODO refresh to previously open list (not favorites as it is now)
     */
    @Override
    protected void onRestart() {
        super.onRestart();

        ActivityResultLauncher<Intent> launcher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result -> {
                    if (result.getResultCode() == RESULT_OK) {
                        Intent data = result.getData();
                    }
                }
        );

        // performs some kind of refresh to update favorites' list.
        // whatever list was on it refreshes into favorites list.
        if (!isUpdated) {
            finish();

            NavigationBar.getUserLists(false, this, launcher, ListPreviewActivity.class, null);
            isUpdated = true;
        }
    }

    public static void setIsUpdated(boolean updated) {
        isUpdated = updated;
    }
}