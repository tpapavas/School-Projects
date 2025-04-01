package com.example.insidethescore;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.PopupMenu;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.activity.EdgeToEdge;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.fragment.app.FragmentContainerView;
import androidx.gridlayout.widget.GridLayout;


import com.example.insidethescore.auxiliary.SearchEngine;
import com.example.insidethescore.music.MusicObjectSelection;
import com.example.insidethescore.music.SearchSelection;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.firebase.firestore.DocumentReference;
import com.google.firebase.firestore.DocumentSnapshot;
import com.google.firebase.firestore.Filter;
import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.firestore.Query;
import com.google.firebase.firestore.QueryDocumentSnapshot;
import com.google.firebase.firestore.QuerySnapshot;

import java.util.HashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class SearchActivity extends AppCompatActivity
        implements PopupMenu.OnMenuItemClickListener {
    static final int NUM_OF_LIST_ITEMS = 6;

    static int currentComposersPage = 1;

    HashMap<String, MusicObjectSelection[]> musicObjectsLists;
    HashMap<String, ToggleButton[]> listButtons;
    HashMap<String, String> searchFields;

    SearchSelection searchSelection;

    GridLayout erasListGrid;
    GridLayout composerListGrid;
    GridLayout musicWorkListGrid;

    FloatingActionButton gotoMusicWorkButton;

    Button filterButton;
    EditText searchEditText;
    ImageButton executeSearchButton;

    private ActivityResultLauncher<Intent> launcher;

    private SearchEngine searchEngine;
    private String filterCollection;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_search);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        // used instead of intents
        launcher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result -> {
                    if (result.getResultCode() == RESULT_OK) {
                        Intent data = result.getData();
                    }
                }
        );

        searchEngine = new SearchEngine();

        filterButton = (Button) findViewById(R.id.filter_button);
        filterCollection = "All";
        searchEditText = (EditText) findViewById(R.id.search_edittext);
        executeSearchButton = (ImageButton) findViewById(R.id.execute_search_button);

        gotoMusicWorkButton = (FloatingActionButton) findViewById(R.id.goto_musicwork_button);

        searchSelection = new SearchSelection();

        musicObjectsLists = new HashMap<>();
        listButtons = new HashMap<>();
        searchFields = new HashMap<>();

        searchFields.put("Eras", "name");
        searchFields.put("Composers", "surname");
        searchFields.put("MusicWorks", "title");

        musicObjectsLists.put("Eras", new MusicObjectSelection[NUM_OF_LIST_ITEMS]);
        musicObjectsLists.put("Composers", new MusicObjectSelection[NUM_OF_LIST_ITEMS]);
        musicObjectsLists.put("MusicWorks", new MusicObjectSelection[NUM_OF_LIST_ITEMS]);

        for (int i = 0; i < NUM_OF_LIST_ITEMS; i++) {
            musicObjectsLists.get("Eras")[i] = new MusicObjectSelection();
            musicObjectsLists.get("Composers")[i] = new MusicObjectSelection();
            musicObjectsLists.get("MusicWorks")[i] = new MusicObjectSelection();
        }

        erasListGrid = findViewById(R.id.eras_list_grid);
        composerListGrid = findViewById(R.id.composers_list_grid);
        musicWorkListGrid = findViewById(R.id.musicworks_list_grid);

        listButtons.put("Eras", new ToggleButton[NUM_OF_LIST_ITEMS]);
        listButtons.put("Composers", new ToggleButton[NUM_OF_LIST_ITEMS]);
        listButtons.put("MusicWorks", new ToggleButton[NUM_OF_LIST_ITEMS]);

        for (int i = 0; i < NUM_OF_LIST_ITEMS; i++) {
            listButtons.get("Eras")[i] = (ToggleButton) erasListGrid.getChildAt(i);
            listButtons.get("Composers")[i] = (ToggleButton) composerListGrid.getChildAt(i);
            listButtons.get("MusicWorks")[i] = (ToggleButton) musicWorkListGrid.getChildAt(i);
        }

        // set onClick listener for eras buttons
        for (int i = 0; i < NUM_OF_LIST_ITEMS; i++) {
            int finalI = i;
            listButtons.get("Eras")[i].setOnClickListener(
                    v -> {
                        clearMusicObjectList(musicObjectsLists.get("Composers"), listButtons.get("Composers"), 0);
                        clearMusicObjectList(musicObjectsLists.get("MusicWorks"), listButtons.get("MusicWorks"), 0);

                        if (listButtons.get("Eras")[finalI].isChecked()) {
                            if (!musicObjectsLists.get("Eras")[finalI].getId().equals("")) {
                                searchSelection.setEra(
                                        musicObjectsLists.get("Eras")[finalI].getId(),
                                        musicObjectsLists.get("Eras")[finalI].getValue());

                                // update composers list to get composers that are from selected era
                                updateObjectList("Composers", "Eras", searchSelection.getEra().getId(),
                                        "era", listButtons.get("Composers"), "surname", musicObjectsLists.get("Composers"));
                            }

                            // uncheck every other eras button
                            for (int k = 0; k < NUM_OF_LIST_ITEMS; k++) {
                                if (k != finalI) {
                                    listButtons.get("Eras")[k].setChecked(false);
                                }
                            }
                        } else {
                            // if an era button gets unchecked, refresh composers.
                            initObjectList("Composers", listButtons.get("Composers"), "surname", musicObjectsLists.get("Composers"), 1);
                        }
                    }
            );
        }

        // set onClick listener for composer buttons
        for (int i = 0; i < NUM_OF_LIST_ITEMS; i++) {
            int finalI = i;
            listButtons.get("Composers")[i].setOnClickListener(
                v -> {
                    clearMusicObjectList(musicObjectsLists.get("MusicWorks"), listButtons.get("MusicWorks"), 0);

                    if (listButtons.get("Composers")[finalI].isChecked()) {
                        if (!musicObjectsLists.get("Composers")[finalI].getId().equals("")) {
                            searchSelection.setComposer(musicObjectsLists.get("Composers")[finalI].getId(), musicObjectsLists.get("Composers")[finalI].getValue());
                            updateObjectList("MusicWorks", "Composers", searchSelection.getComposer().getId(),
                                    "composer", listButtons.get("MusicWorks"), "title", musicObjectsLists.get("MusicWorks"));
                        }

                        for (int k = 0; k < NUM_OF_LIST_ITEMS; k++) {
                            if (k != finalI) {
                                listButtons.get("Composers")[k].setChecked(false);
                            }
                        }
                    }
                }
            );
        }

        // set onClick listener for musicwork buttons
        for (int i = 0; i < NUM_OF_LIST_ITEMS; i++) {
            int finalI = i;
            listButtons.get("MusicWorks")[i].setOnClickListener(
                    v -> {
                        if (listButtons.get("MusicWorks")[finalI].isChecked()) {
                            if (!musicObjectsLists.get("MusicWorks")[finalI].getId().equals("")) {
                                searchSelection.setMusicWork(musicObjectsLists.get("MusicWorks")[finalI].getId(), musicObjectsLists.get("MusicWorks")[finalI].getValue());
                                gotoMusicWorkButton.setEnabled(true);
                            }

                            for (int k = 0; k < NUM_OF_LIST_ITEMS; k++) {
                                if (k != finalI) {
                                    listButtons.get("MusicWorks")[k].setChecked(false);
                                }
                            }
                        } else {
                            gotoMusicWorkButton.setEnabled(false);
                        }
                    }
            );
        }

        initObjectList("Eras", listButtons.get("Eras"), "name", musicObjectsLists.get("Eras"), 1);
        initObjectList("Composers", listButtons.get("Composers"), "surname", musicObjectsLists.get("Composers"), 1);
    }

    /**
     * If a category (eras, composers, musicworks) is selected,
     * search database for objects (eras, composers, musicworks)
     * that matches the typed text in the way searchEngine defines.
     * @param view the view inside whitch the click was performed on the search button
     */
    public void executeSearch (View view) {
        String searchToken = searchEditText.getText().toString();

        searchEngine.generateSearchLimits(searchToken);

        Log.d("executeSearch", "filterCollection: " + filterCollection);

        if (!filterCollection.equals("All")) {  // a category is selected
            if (!searchToken.equals("")) {
                executeSearchQuery(filterCollection, searchFields.get(filterCollection),
                        searchEngine.getLowerLimitToken(), searchEngine.getUpperLimitToken(),
                        musicObjectsLists.get(filterCollection), listButtons.get(filterCollection));
            } else {
                clearAll(view);
            }
        } else {
            Toast.makeText(this, "Choose a category to search", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Get from db NUM_OF_LIST_ITEMS (6 here) documents of the
     * specified category-collection to fill the respective buttons.
     * @param collectionToInit which db collection we want to initialize ("Eras", "Composers", "MusicWorks")
     * @param buttons activity's buttons, respective to collection, that we want to fill with info
     * @param setField which field we search the db's collection documents for (e.g. for "Composers" it's surname)
     * @param musicObjectSelections objects that keep more info about the button's items.
     * @param pageToShow which page of documents to display on buttons (page 1, first 6 docs from db, page 2, next 6 docs etc).
     */
    private void initObjectList(String collectionToInit, ToggleButton[] buttons,
                                String setField, MusicObjectSelection[] musicObjectSelections,
                                int pageToShow) {
        clearMusicObjectList(musicObjectSelections, buttons, 0);

        FirebaseFirestore db = FirebaseFirestore.getInstance();

        Query objectCollectionQuery = db.collection(collectionToInit).limit(NUM_OF_LIST_ITEMS * pageToShow);
        objectCollectionQuery
                .get()
                .addOnCompleteListener(new OnCompleteListener<QuerySnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<QuerySnapshot> task) {
                        if (task.isSuccessful()) {
                            int i = 0;
                            int iLowLimit = NUM_OF_LIST_ITEMS * (pageToShow-1);
                            int iHighLimit = iLowLimit + NUM_OF_LIST_ITEMS - 1;
                            for (QueryDocumentSnapshot document : task.getResult()) {
                                if (i >= iLowLimit && i <= iHighLimit) {
                                    if (document.exists()) {
                                        Log.d("initObjectList", "DocumentSnapshot data: " + document.getData().get(setField));
                                        buttons[i-iLowLimit].setText(document.getData().get(setField).toString());
                                        buttons[i-iLowLimit].setTextOff(document.getData().get(setField).toString());
                                        buttons[i-iLowLimit].setTextOn(document.getData().get(setField).toString());

                                        musicObjectSelections[i-iLowLimit].set(document.getId(), document.getData().get(setField).toString());
                                    } else {
                                        Log.d("initObjectList", "No such document");
                                    }
                                }

                                i++;
                            }
                        } else {
                            Log.d("initObjectList", "get failed with ", task.getException());
                        }
                    }
                });
    }

    /**
     *
     * @param collectionToUpdate which db collection we want to update ("Eras", "Composers", "MusicWorks")
     * @param fieldCollection name of the collection that comes up as a field in collectionToUpdate docs
     * @param collectionObjectId collection object id that matches with collectionToUpdate docs field
     * @param searchField the field we match collectionObjectId with on collectionToUpdate docs.
     * @param buttons buttons to update after query finishes
     * @param setField the field we fill the buttons with
     * @param musicObjectSelections objects that keep more info about the button's items.
     */
    private void updateObjectList(String collectionToUpdate, String fieldCollection,
                                  String collectionObjectId, String searchField,
                                  ToggleButton[] buttons, String setField, MusicObjectSelection[] musicObjectSelections) {
        FirebaseFirestore db = FirebaseFirestore.getInstance();

        Log.d("updateObjectList", "inside update");

        // get documents that searchField matches fieldCollection's doc with id = collectionObjectId
        DocumentReference ref = db.document("/" + fieldCollection + "/" + collectionObjectId);
        Query objectCollectionQuery = db.collection(collectionToUpdate).where(Filter.equalTo(searchField, ref)).limit(NUM_OF_LIST_ITEMS);
        objectCollectionQuery
                .get()
                .addOnCompleteListener(new OnCompleteListener<QuerySnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<QuerySnapshot> task) {
                        if (task.isSuccessful()) {
                            int i = 0;
                            for (QueryDocumentSnapshot document : task.getResult()) {
                                if (document.exists()) {
                                    Log.d("updateObjectList", "DocumentSnapshot data: " + document.getData().get(setField));
                                    buttons[i].setText(document.getData().get(setField).toString());
                                    buttons[i].setTextOff(document.getData().get(setField).toString());
                                    buttons[i].setTextOn(document.getData().get(setField).toString());

                                    musicObjectSelections[i].set(document.getId(), document.getData().get(setField).toString());

                                    i++;
                                } else {
                                    Log.d("updateObjectList", "No such document");
                                }
                            }

                            clearMusicObjectList(musicObjectSelections, buttons, i);
                        } else {
                            Log.d("updateObjectList", "get failed with ", task.getException());
                        }
                    }
                });
    }

    private void clearMusicObjectList(MusicObjectSelection[] musicObjectSelections, ToggleButton[] buttons, int offset) {
        for (int i = offset; i < NUM_OF_LIST_ITEMS; i++) {
            musicObjectSelections[i].set("", "");
            buttons[i].setChecked(false);
            buttons[i].setText("");
            buttons[i].setTextOff("");
            buttons[i].setTextOn("");
        }
        gotoMusicWorkButton.setEnabled(false);
    }

    private void setMusicObjectItem(MusicObjectSelection[] musicObjectSelection, ToggleButton[] buttons, int index,
                                    String newText, String newId, String newValue) {
        buttons[index].setText(newText);
        buttons[index].setTextOff(newText);
        buttons[index].setTextOn(newText);

        musicObjectSelection[index].set(newId, newValue);
    }

    /***
     * Get from db the items that match the typed text.
     * That is, get the items from db where their searchField (e.g. "surname") >= lowerLimit
     * and searchField <= upperLimit.
     * E.g. if typed text is "Bee", and collection is "Composers", searchField is "surname",
     * bring from db the composers whose surname is >= Be and <= Bf, like Beethoven, Berg.
     * @param collection the collection to search items on
     * @param searchField the field to compare lowerLimit and upperLimit with
     * @param lowerLimit the string that item's field must be bigger than.
     * @param upperLimit the string that item's field must be smaller than.
     * @param musicObjectSelection the objects that match the buttons (eras, composers, etc)
     * @param buttons the buttons of the defined category on screen.
     */
    public void executeSearchQuery(String collection, String searchField,
                                   String lowerLimit, String upperLimit,
                                   MusicObjectSelection[] musicObjectSelection, ToggleButton[] buttons) {
        FirebaseFirestore db = FirebaseFirestore.getInstance();

//        Log.d("updateObjectList", "inside update");

        // find documents that match the typed text
        Query objectCollectionQuery = db.collection(collection)
                .where(Filter.and(
                        Filter.greaterThanOrEqualTo(searchField, lowerLimit),
                        Filter.lessThanOrEqualTo(searchField, upperLimit)))
                .limit(NUM_OF_LIST_ITEMS);
        objectCollectionQuery
                .get()
                .addOnCompleteListener(new OnCompleteListener<QuerySnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<QuerySnapshot> task) {
                        if (task.isSuccessful()) {
                            clearMusicObjectList(musicObjectsLists.get("Eras"), listButtons.get("Eras"), 0);
                            clearMusicObjectList(musicObjectsLists.get("Composers"), listButtons.get("Composers"), 0);
                            clearMusicObjectList(musicObjectsLists.get("MusicWorks"), listButtons.get("MusicWorks"), 0);

                            int i = 0;
                            for (QueryDocumentSnapshot document : task.getResult()) {
                                if (document.exists()) {
                                    if (collection.equals("MusicWorks")) {
                                        // for music works we need to add in mw buttons the composer's surname
                                        // in order to distinguish MWs with similar name from different composers

                                        DocumentReference composerDocRef = (DocumentReference) document.getData().get("composer");

                                        // run query for composer name
                                        int finalI = i;
                                        composerDocRef
                                                .get()
                                                .addOnCompleteListener(task1 -> {
                                                    if (task1.isSuccessful()) {
                                                        DocumentSnapshot composerDocSnap = task1.getResult();
                                                        if (composerDocSnap.exists()) {
                                                            String fieldValue = "("
                                                                    .concat(composerDocSnap.getData().get("surname").toString())
                                                                    .concat(") ")
                                                                    .concat(document.getData().get(searchField).toString());

                                                            setMusicObjectItem(musicObjectSelection, buttons, finalI,
                                                                    fieldValue,
                                                                    document.getId(),
                                                                    fieldValue);
                                                        }
                                                    }
                                                });
                                    } else {
                                        // update buttons with newly retrieved objects
                                        setMusicObjectItem(musicObjectSelection, buttons, i,
                                                document.getData().get(searchField).toString(),
                                                document.getId(),
                                                document.getData().get(searchField).toString());
                                    }
                                    i++;
                                } else {
                                    Log.d("ReadMusic", "No such document");
                                }
                            }
                        } else {
                            Log.d("ReadMusic", "get failed with ", task.getException());
                        }
                    }
                });
    }

    public void goToMusicWork(View view) {
        Intent i = new Intent(this, ReadMusicActivity.class);

        //Pass data to the SayHelloNewScreen Activity through the Intent
        CharSequence musicWorkId = searchSelection.getMusicWork().getId();
        i.putExtra("musicWorkId", musicWorkId);

        // retrieve user's lists and start ReadMusicActivity
        // we need user's lists to check if this music work is in "Favorites" list
        // to set the heart icon (which represents existence in favs list).
        NavigationBar.getUserLists(false, this, launcher, ReadMusicActivity.class, i.getExtras());

        //Ask Android to start the new Activity
//        launcher.launch(i);

        overridePendingTransition(R.anim.slide_in_right, R.anim.slide_out_left);
    }

    public void showPopup(View view) {
        PopupMenu popup = new PopupMenu(this,view);
        popup.setOnMenuItemClickListener(this);
        popup.inflate(R.menu.search_filters_menu);
        popup.show();
    }

    @Override
    public boolean onMenuItemClick(MenuItem item) {
        searchEditText.setEnabled(true);

        if (item.getItemId() == R.id.eras_search_option) {
            filterButton.setText(getString(R.string.eras_list_textview) + ":");
            filterCollection = "Eras";
        } else if (item.getItemId() == R.id.composers_search_option) {
            filterButton.setText(getString(R.string.composers_list_textview) + ":");
            filterCollection = "Composers";
        } else if (item.getItemId() == R.id.musicworks_search_option) {
            filterButton.setText(getString(R.string.musicworks_list_textview) + ":");
            filterCollection = "MusicWorks";
        } else {
            filterCollection = "All";
            searchEditText.setEnabled(false);
            return false;
        }

        return true;
    }

    public void clearAll(View view) {
        initObjectList("Eras", listButtons.get("Eras"), "name", musicObjectsLists.get("Eras"), 1);
        initObjectList("Composers", listButtons.get("Composers"), "surname", musicObjectsLists.get("Composers"), 1);
        clearMusicObjectList(musicObjectsLists.get("MusicWorks"), listButtons.get("MusicWorks"), 0);

        filterButton.setText("All");
        filterCollection = "All";
        searchEditText.setText("");
        searchEditText.setEnabled(false);
        currentComposersPage = 1;
    }

    public void echoHelp(View view) {
        String helpMsg = "Info\n" +
                "Select a music work to view its details.\n" +
                "You can select a period to get a period's composer \n" +
                "and select a composer to get his/her works";

        FragmentContainerView helpFragment = (FragmentContainerView) findViewById(R.id.help_msg_fragment);
        TextView helpMsgTextView = (TextView) findViewById(R.id.help_msg_textview);
        helpMsgTextView.setText(helpMsg);

        if (helpFragment.getVisibility() == View.INVISIBLE) {
            helpFragment.setVisibility(View.VISIBLE);
        } else {
            helpFragment.setVisibility(View.INVISIBLE);
        }
    }

    public void getPrevCompPage(View view) {
        if (currentComposersPage - 1 > 0) {
            initObjectList("Composers", listButtons.get("Composers"), "surname", musicObjectsLists.get("Composers"), (currentComposersPage-1));

            currentComposersPage--;
        }
    }

    public void getNextCompPage(View view) {
        initObjectList("Composers", listButtons.get("Composers"), "surname", musicObjectsLists.get("Composers"), (currentComposersPage+1));
        currentComposersPage++;
    }
}