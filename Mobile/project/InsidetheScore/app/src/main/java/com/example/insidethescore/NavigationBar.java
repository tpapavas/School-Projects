package com.example.insidethescore;

import static android.app.Activity.RESULT_OK;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;

import com.example.insidethescore.auxiliary.ScoreDatabaseManager;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.firestore.Filter;
import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.firestore.Query;
import com.google.firebase.firestore.QueryDocumentSnapshot;
import com.google.firebase.firestore.QuerySnapshot;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link NavigationBar#newInstance} factory method to
 * create an instance of this fragment.
 */
public class NavigationBar extends Fragment {
    ImageButton searchButton;
    ImageButton libraryButton;
    ImageButton homeButton;

    private ActivityResultLauncher<Intent> launcher;

    public NavigationBar() {}

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     * @return A new instance of fragment NavigationBar.
     */
    // TODO: Rename and change types and number of parameters
    public static NavigationBar newInstance() {
        NavigationBar fragment = new NavigationBar();
        Bundle args = new Bundle();
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        launcher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result -> {
                    if (result.getResultCode() == RESULT_OK) {
                        Intent data = result.getData();
                    }
                }
        );
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_navigation_bar, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        searchButton = (ImageButton) view.findViewById(R.id.search_button);
        libraryButton = (ImageButton) view.findViewById(R.id.library_button);
        homeButton = (ImageButton) view.findViewById(R.id.home_button);

        searchButton.setOnClickListener(
            v -> {
                Intent i = new Intent(getActivity(), SearchActivity.class);

                launcher.launch(i);
            }
        );

        libraryButton.setOnClickListener(
                v -> {
                    Log.d("getUserLists", "In function");

                    // fetch user lists and start LibraryActivity
                    getUserLists(true, getContext(), launcher, LibraryActivity.class, null);
                }
        );

        homeButton.setOnClickListener(
                v -> {
                    Intent i = new Intent(getActivity(), MainActivity.class);
                    launcher.launch(i);
                }
        );
    }

    /**
     * Get this user's lists from db.
     * It first retrieves lists document id from db and then
     * it calls getUserListsInfo to complete the whole objective
     * (get user's list info and possibly start a new activity)
     * @param startNewActivity if we want to start a new activity on query completion
     * @param context the context from which we will call the new activity
     * @param launcher ActivityResultLauncher<Intent> object to start new activity from
     * @param newClass the new class that we want to start
     * @param extras any extra data to pass to newClass
     */
    public static void getUserLists(boolean startNewActivity, Context context, ActivityResultLauncher<Intent> launcher, Class<?> newClass, Bundle extras) {
        // get user lists
        FirebaseFirestore db = FirebaseFirestore.getInstance();

        Query objectCollectionQuery = db.collection("Lists")
                .where(Filter.equalTo("username", ScoreDatabaseManager.getUsername()));

        objectCollectionQuery
                .get()
                .addOnCompleteListener(new OnCompleteListener<QuerySnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<QuerySnapshot> task) {
                        if (task.isSuccessful()) {
//                            int i = 0;
                            for (QueryDocumentSnapshot document : task.getResult()) {
                                if (document.exists()) {
                                    ScoreDatabaseManager.setUserListsDocumentId(document.getId());

                                    Log.d("ScoreDatabaseManager", "UserLists: " + ScoreDatabaseManager.getUserListsDocumentId());
//                                    i++;
                                } else {
                                    Log.d("getUserLists", "No such document");
                                }
                            }

                            ScoreDatabaseManager.setUserListsCollection(
                                    db.collection("Lists/" + ScoreDatabaseManager.getUserListsDocumentId() + "/UserLists"));

                            getUserListsInfo(startNewActivity, context, launcher, newClass, extras);

                            Log.d("ScoreDatabaseManager", "userListsCollection path: " + ScoreDatabaseManager.getUserListsCollection());
                        } else {
                            Log.d("getUserLists", "get failed with ", task.getException());
                        }
                    }
                });
    }

    /**
     * Get info for every list in user's lists document's collection in db.
     * @param startNewActivity if we want to start a new activity on query completion
     * @param context the context from which we will call the new activity
     * @param launcher ActivityResultLauncher<Intent> object to start new activity from
     * @param newClass the new class that we want to start
     * @param extras any extra data to pass to newClass
     */
    private static void getUserListsInfo(boolean startNewActivity, Context context, ActivityResultLauncher<Intent> launcher,
                                         Class<?> newClass, Bundle extras) {
        ScoreDatabaseManager.getUserListsCollection()
                .get()
                .addOnCompleteListener(new OnCompleteListener<QuerySnapshot>() {
                    @Override
                    public void onComplete(@NonNull Task<QuerySnapshot> task) {
                        if (task.isSuccessful()) {
                            ScoreDatabaseManager.numOfLists = 0;
                            int i = 0;
                            for (QueryDocumentSnapshot document : task.getResult()) {
                                if (document.exists()) {
                                    ScoreDatabaseManager.listsTitles[i] =  document.getData().get("title").toString();
                                    ScoreDatabaseManager.listsDetails[i] =  document.getData().get("details").toString();

                                    // store "Favorites" list data in special place
                                    if (document.getData().get("title").toString().equals("Favorites")) {
                                        ScoreDatabaseManager.setUserFavListId(document.getId());
                                    }

                                    Log.d("getUserLists", "UserListsTitles: " + ScoreDatabaseManager.listsTitles[i]);
                                    i++;
                                    ScoreDatabaseManager.numOfLists++;
                                } else {
                                    Log.d("getUserListsInfo", "No such document");
                                }
                            }

                            if (startNewActivity) {
//                                Intent intent = new Intent(getActivity(), LibraryActivity.class);
//                                Intent intent = new Intent(context, LibraryActivity.class);
                                Intent intent = new Intent(context, newClass);
                                launcher.launch(intent);
                            } else {
//                                RecyclerAdapter.ViewHolder.getUserListMusicWorks("Favorites", context, ListPreviewActivity.class);
                                RecyclerAdapter.ViewHolder.getUserListMusicWorks("Favorites", context, newClass, extras);
                            }
                        } else {
                            Log.d("getUserListsInfo", "get failed with ", task.getException());
                        }
                    }
                });
    }
}