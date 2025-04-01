package com.example.insidethescore;

import android.os.Bundle;
import android.text.Html;
import android.text.Spanned;
import android.text.SpannedString;
import android.text.method.LinkMovementMethod;
import android.util.Base64;
import android.util.Log;
import android.view.View;
import android.webkit.WebView;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.example.insidethescore.auxiliary.ScoreDatabaseManager;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.firestore.DocumentReference;
import com.google.firebase.firestore.DocumentSnapshot;
import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.storage.FirebaseStorage;
import com.google.firebase.storage.StorageReference;

public class ReadMusicActivity extends AppCompatActivity {
    private TextView musicWorkTitleTextView;
    private TextView musicWorkYearTextView;
    private TextView musicWorkDescriptionTextView;

    private TextView musicWorkLinkTextView;
    private ImageButton favButton;

    private CharSequence musicWorkId;

    private static boolean isFavButtonPushed = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_read_music);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        musicWorkTitleTextView = findViewById(R.id.musicwork_title_textview);
        musicWorkYearTextView = findViewById(R.id.musicwork_year_textview);
        musicWorkDescriptionTextView = findViewById(R.id.musicwork_description_textview);
        musicWorkLinkTextView = findViewById(R.id.musicwork_link_textview);

        favButton = (ImageButton) findViewById(R.id.add_to_favs_button);
        isFavButtonPushed = false;

        //Get Bundle from the Intent
        Bundle extras = getIntent().getExtras();
//        //If there are data passed in the Intent
        if (extras != null) {
            //Retrieve data passed in the Intent
            musicWorkId = extras.getCharSequence("musicWorkId");
            //For debugging: print in the Logcat (Debug level)
            Log.d("ReadMusicActivity.java", musicWorkId.toString());

            // check if this music work is in "Favorites" to set the heart button appropriately
            int numOfFavsMWs = ScoreDatabaseManager.listMusicWorksIds.length;
            int i = 0;
            while (i < numOfFavsMWs && ScoreDatabaseManager.listMusicWorksIds[i] != null) {
                if (ScoreDatabaseManager.listMusicWorksIds[i].equals(musicWorkId.toString())) {
                    favButton.setImageDrawable(getDrawable(R.drawable.heart_icon_filled));
                    isFavButtonPushed = true;

                    break;
                }

                i++;
            }

            FirebaseFirestore db = FirebaseFirestore.getInstance();

            // get this music work's info by searching the db for its id
            DocumentReference documentReference = db.collection("MusicWorks").document(musicWorkId.toString());
            documentReference
                .get()
                .addOnCompleteListener(new OnCompleteListener<DocumentSnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<DocumentSnapshot> task) {
                        if (task.isSuccessful()) {
                            DocumentSnapshot document = task.getResult();
                            if (document.exists()) {
                                Log.d("ReadMusic", "DocumentSnapshot data: " + document.getData().get("title"));

                                musicWorkTitleTextView.setText(document.getData().get("title").toString());
                                musicWorkYearTextView.setText(document.getData().get("year").toString());
                                musicWorkDescriptionTextView.setText(document.getData().get("description").toString());

//                                // considering display the details of music work in html for nicer preview.
//                                String html = "<!DOCTYPE html>\n" +
//                                        "<html>\n" +
//                                        "<body>\n" +
//                                        "\n" +
//                                        "<h1>My First Heading</h1>\n" +
//                                        "\n" +
//                                        "<p>My first paragraph.</p>\n" +
//                                        "\n" +
//                                        "</body>\n" +
//                                        "</html>";
//
                                musicWorkLinkTextView.setText(document.getData().get("link").toString());
//
//                                WebView webView = findViewById(R.id.webview);
//                                String encodedHtml = Base64.encodeToString(html.getBytes(), Base64.NO_PADDING);
//                                webView.loadData(encodedHtml, "text/html", "base64");

                                // make link clickable
                                musicWorkYearTextView.setMovementMethod(LinkMovementMethod.getInstance());
                            } else {
                                Log.d("ReadMusic", "No such document");
                            }
                        } else {
                            Log.d("ReadMusic", "get failed with ", task.getException());
                        }
                    }
                });

        }

        TextView textView = findViewById(R.id.link_textview);
        textView.setMovementMethod(LinkMovementMethod.getInstance());
    }

    @Override
    public void finish() {
        super.finish();

        overridePendingTransition(R.anim.slide_in_left, R.anim.slide_out_right);
    }

    public void addToFavorites(View view) {
        FirebaseFirestore db = FirebaseFirestore.getInstance();

        ListPreviewActivity.setIsUpdated(false);

        if (isFavButtonPushed) {
            // remove music work from "Favorites" list
            favButton.setImageDrawable(getDrawable(R.drawable.heart_icon_border));
            isFavButtonPushed = false;

            ScoreDatabaseManager.getUserFavListMusicWorks().remove(db.document("/MusicWorks/" + musicWorkId.toString()));

            db.collection("Lists").document(ScoreDatabaseManager.getUserListsDocumentId())
                    .collection("UserLists").document(ScoreDatabaseManager.getUserFavListId())
                    .update("musicworks", ScoreDatabaseManager.getUserFavListMusicWorks());

            Toast.makeText(this, "Work removed from fav list", Toast.LENGTH_SHORT).show();
        } else {
            // add music work to "Favorites" list
            favButton.setImageDrawable(getDrawable(R.drawable.heart_icon_filled));
            isFavButtonPushed = true;

            ScoreDatabaseManager.getUserFavListMusicWorks().add(db.document("/MusicWorks/" + musicWorkId.toString()));

            db.collection("Lists").document(ScoreDatabaseManager.getUserListsDocumentId())
                    .collection("UserLists").document(ScoreDatabaseManager.getUserFavListId())
                    .update("musicworks", ScoreDatabaseManager.getUserFavListMusicWorks());

            Toast.makeText(this, "Work added to fav list", Toast.LENGTH_SHORT).show();
        }
    }
}