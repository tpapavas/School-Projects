package com.example.insidethescore;

import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.insidethescore.auxiliary.ScoreDatabaseManager;

public class ListPreviewRecyclerAdapter extends RecyclerView.Adapter<ListPreviewRecyclerAdapter.ViewHolder> {
        static class ViewHolder extends RecyclerView.ViewHolder {
        TextView itemTitle;
        TextView itemDetail;
        public ViewHolder(View itemView) {
            super(itemView);
            itemTitle = itemView.findViewById(R.id.list_musicwork_title);
            itemDetail = itemView.findViewById(R.id.list_musicwork_details);

            itemView.setOnClickListener(
                    v -> {
                        Context context = itemView.getContext();
                        Intent i = new Intent(context, ReadMusicActivity.class);

                        CharSequence musicWorkId = ScoreDatabaseManager.listMusicWorksIds[getAbsoluteAdapterPosition()];
                        i.putExtra("musicWorkId", musicWorkId);

                        //Ask Android to start the new Activity
//                        launcher.launch(i);
                        context.startActivity(i);
                    }
            );
        }
    }

    @NonNull
    @Override
    public ListPreviewRecyclerAdapter.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.music_work_layout, parent, false);
        return new ViewHolder(v);
    }

    @Override
    public void onBindViewHolder(@NonNull ListPreviewRecyclerAdapter.ViewHolder holder, int position) {
        if (ScoreDatabaseManager.getNumOfLists() > 0) {
            holder.itemTitle.setText(ScoreDatabaseManager.listMusicWorksTitles[position]);
            holder.itemDetail.setText(ScoreDatabaseManager.listMusicWorksDetails[position]);
        }
    }

    @Override
    public int getItemCount() {
        return ScoreDatabaseManager.getNumOfListMusicWorks();
    }
}
