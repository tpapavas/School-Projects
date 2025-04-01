package com.example.insidethescore.auxiliary;

import com.google.firebase.firestore.CollectionReference;
import com.google.firebase.firestore.DocumentReference;

import java.util.ArrayList;

public class ScoreDatabaseManager {
    private static final int NUM_OF_LIST_ITEMS = 6;  // number of composers (eras, musicworks) buttons on SearchActivity.
    public static final int MAX_USER_LISTS = 50;  // maximum number of musicworks (mw) in a list

    // instead of login
    private static final String userId = "XCmvItqgYEhh8abZHSJn";
    private static final String username = "user1";

    private static String userListsDocumentId;  // id of current user's Lists' document in db
    public static CollectionReference userListsCollection; // reference to user's UserLists collection in db

    public static String[] listsTitles;  // titles of user's lists
    public static String[] listsDetails;  // details of user's lists
    public static String[] listMusicWorksIds;  // ids of musicworks (mw) of currently selected list
    public static String[] listMusicWorksTitles;  // titles of musicworks (mw) of currently selected list
    public static String[] listMusicWorksDetails;  // details of musicworks (mw) of currently selected list
    public static int numOfLists;  // number of user's lists
    public static int numOfListMusicWorks;  // number of currently selected user's list
    private static String currentList;  // name of currently selected list
    private static ArrayList<DocumentReference> currentListMusicWorks;  // array of refernces to db documents of MWs of currently selected list

    private static String userFavListId;  // id of user's "Favorites" list
    private static ArrayList<DocumentReference> userFavListMusicWorks;  // array of refernces to db documents of MWs of "Favorites" list

    public ScoreDatabaseManager() {
        listsTitles = new String[MAX_USER_LISTS];
        listsDetails = new String[MAX_USER_LISTS];

        listMusicWorksTitles = new String[MAX_USER_LISTS];
        listMusicWorksDetails = new String[MAX_USER_LISTS];
        listMusicWorksIds = new String[MAX_USER_LISTS];

        currentListMusicWorks = new ArrayList<>();
        userFavListMusicWorks = new ArrayList<>();

        numOfLists = 0;
        numOfListMusicWorks = 0;
    }

    public static int getNumOfLists() {
        return numOfLists;
    }

    public static int getNumOfListMusicWorks() { return numOfListMusicWorks; }

    public static String getUsername() { return username; }

    public static void setUserListsDocumentId(String id) {
        userListsDocumentId = id;
    }

    public static String getUserListsDocumentId() {
        return userListsDocumentId;
    }

    public static void setUserListsCollection(CollectionReference userListsCol) {
        userListsCollection = userListsCol;
    }

    public static CollectionReference getUserListsCollection() {
        return userListsCollection;
    }

    public static void setCurrentList(String list) {
        currentList = list;
    }

    public static String getCurrentList() {
        return currentList;
    }

    public static void setCurrentListMusicWorks(ArrayList<DocumentReference> musicworks) {
        currentListMusicWorks = musicworks;
    }

    public static ArrayList<DocumentReference> getCurrentListMusicWorks() {
        return currentListMusicWorks;
    }

    public static void setUserFavListId(String id) {
        userFavListId = id;
    }

    public static String getUserFavListId() {
        return userFavListId;
    }

    public static void setUserFavListMusicWorks(ArrayList<DocumentReference> musicworks) {
        userFavListMusicWorks = musicworks;
    }

    public static ArrayList<DocumentReference> getUserFavListMusicWorks() {
        return userFavListMusicWorks;
    }
}
