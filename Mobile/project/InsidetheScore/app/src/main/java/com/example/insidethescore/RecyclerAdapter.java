package com.example.insidethescore;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.insidethescore.auxiliary.ScoreDatabaseManager;
import com.google.firebase.firestore.DocumentReference;
import com.google.firebase.firestore.DocumentSnapshot;
import com.google.firebase.firestore.Filter;
import com.google.firebase.firestore.QueryDocumentSnapshot;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;

public class RecyclerAdapter extends RecyclerView.Adapter<RecyclerAdapter.ViewHolder> {
    static class ViewHolder extends RecyclerView.ViewHolder {
        TextView itemTitle;
        TextView itemDetail;
        public ViewHolder(View itemView) {
            super(itemView);
            itemTitle = itemView.findViewById(R.id.item_title);
            itemDetail = itemView.findViewById(R.id.item_detail);

            itemView.setOnClickListener(
                    v -> {
                        getUserListMusicWorks(itemTitle.getText().toString(), v.getContext(), ListPreviewActivity.class, null);
                    }
            );
        }

        /**
         * get from db the music works of the selected list.
         * It calls getUserListMusicWorksComposers, which then calls getUserListMusicWorksInfo
         * in order to complete the whole objective (get music works of a list
         * and relative composers info and open ListPreviewActivity).
         * @param currList the list we want to get its music works.
         * @param context the contex of execution
         * @param newClass the class-activity that's going to start after db queries completion
         * @param extras extra data to pass to new activity
         */
        public static void getUserListMusicWorks(String currList, Context context, Class<?> newClass, Bundle extras) {
            String list;

//                if (!currList.equals("")) {
                list = currList;
//                } else {
//                    list = itemTitle.getText().toString();
//                }

            Log.d("getUserListMusicWorks", "For list " + list);

            ScoreDatabaseManager.setCurrentList(list);

            ScoreDatabaseManager.getUserListsCollection()
                    .where(Filter.equalTo("title", ScoreDatabaseManager.getCurrentList()))
                    .get()
                    .addOnCompleteListener(task -> {
                        if (task.isSuccessful()) {
                            for (QueryDocumentSnapshot document : task.getResult()) {
                                if (document.exists()) {
                                    ScoreDatabaseManager.setCurrentListMusicWorks((ArrayList<DocumentReference>) document.getData().get("musicworks"));

                                    if (currList.equals("Favorites")) {
                                        ScoreDatabaseManager.setUserFavListMusicWorks((ArrayList<DocumentReference>) document.getData().get("musicworks"));
                                    }

                                    // run query for composers names;
                                    getUserListMusicWorksComposers(list, context, newClass, extras);

//                                        getUserListMusicWorksInfo();
                                } else {
                                    Log.d("ReadMusic", "No such document");
                                }
                            }
                        } else {
                            Log.d("ReadMusic", "get failed with ", task.getException());
                        }
                    });
        }

        /**
         * Get from db info about the composers of previously retrieved music works.
         * It calls getUserListMusicWorksInfo in order to complete the superior objective
         * (get music works of a list and relative composers info and open ListPreviewActivity).
         * @param currList
         * @param context
         * @param newClass
         * @param extras
         */
        private static void getUserListMusicWorksComposers(String currList, Context context, Class<?> newClass, Bundle extras) {
            AtomicInteger tasksFinished = new AtomicInteger();

            ArrayList<DocumentReference> docRef = ScoreDatabaseManager.getCurrentListMusicWorks();

            // retrieve one music work at a time
            for (int i = 0; i < docRef.size(); i++) {
                int finalI = i;
                docRef.get(i)
                        .get()
                        .addOnCompleteListener(task -> {
                            if (task.isSuccessful()) {
                                DocumentSnapshot document = task.getResult();
                                if (document.exists()) {
                                    DocumentReference composerDocRef = (DocumentReference) document.getData().get("composer");

                                    // run query for composer's name
                                    composerDocRef
                                            .get()
                                            .addOnCompleteListener(task1 -> {
                                                if (task1.isSuccessful()) {
                                                    DocumentSnapshot composerDocSnap = task1.getResult();
                                                    if (composerDocSnap.exists()) {
                                                        ScoreDatabaseManager.listMusicWorksDetails[finalI] =
                                                                composerDocSnap.getData().get("name").toString()
                                                                        .concat(" ")
                                                                        .concat(composerDocSnap.getData().get("surname").toString());
                                                    }
                                                }

                                                tasksFinished.getAndIncrement();

                                                if (tasksFinished.get() == docRef.size()) {
                                                    getUserListMusicWorksInfo(currList, context, newClass, extras);
                                                }
                                            });
                                }
                            }
                        });
            }

        }

        /**
         * For every music work in current list, retrieve information from db.
         * Also, start ListPreviewActivity to view current list's music works info.
         * @param currList the list we want to get its music works' information
         * @param context the context from which we will call newClass
         * @param newClass the class-activity we want to start after query finished
         * @param extras any extra data we want to pass through the intent to newClass
         */
        private static void getUserListMusicWorksInfo(String currList, Context context, Class<?> newClass, Bundle extras) {
            ScoreDatabaseManager.numOfListMusicWorks = 0;
            AtomicInteger tasksFinished = new AtomicInteger();

            ArrayList<DocumentReference> docRef = ScoreDatabaseManager.getCurrentListMusicWorks();

            // get a music work at a time
            for (int i = 0; i < docRef.size(); i++) {
                int finalI = i;
                docRef.get(i)
                        .get()
                        .addOnCompleteListener(task1 -> {
                            if (task1.isSuccessful()) {
                                DocumentSnapshot document1 = task1.getResult();
                                if (document1.exists()) {
                                    // update variables with current list's music works info
                                    ScoreDatabaseManager.listMusicWorksTitles[finalI] = document1.getData().get("title").toString();
                                    ScoreDatabaseManager.listMusicWorksIds[finalI] = document1.getId().toString();
                                    ScoreDatabaseManager.numOfListMusicWorks++;

                                    Log.d("getUserListMusicWorks", "ListMusicWorks: " + ScoreDatabaseManager.numOfListMusicWorks);
                                }
                            }

                            tasksFinished.getAndIncrement();

                            // make sure all music works info are retrieved.
                            if (tasksFinished.get() == docRef.size()) {
                                Intent intent = new Intent(context, newClass);

                                if (extras != null) {
                                    intent.putExtras(extras);
                                }

                                context.startActivity(intent);
                            }
                        });
            }
        }
    }

    @NonNull
    @Override
    public RecyclerAdapter.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.music_list_layout, parent, false);
        return new ViewHolder(v);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerAdapter.ViewHolder holder, int position) {
        if (ScoreDatabaseManager.getNumOfLists() > 0) {
            holder.itemTitle.setText(ScoreDatabaseManager.listsTitles[position]);
            holder.itemDetail.setText(ScoreDatabaseManager.listsDetails[position]);
        }
    }

    @Override
    public int getItemCount() {
        return ScoreDatabaseManager.getNumOfLists();
    }
}
