#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 50
#define MAX_TITLE 100
#define MAX_GENRE 30
#define MAX_GENRES 10
#define MAX_USERS 50

// Rating Structure (for individual user ratings)
typedef struct Rating {
    int userId;
    int rating;  // 1-5 stars
    struct Rating* next;
} Rating;

// Movie Structure
typedef struct Movie {
    int id;
    char title[MAX_TITLE];
    char genre[MAX_GENRE];
    int year;
    float ratingSum;
    int ratingCount;
    Rating* ratings;  // Linked list of all ratings
    struct Movie* next;  // For hash table chaining
} Movie;

// Hash Table for Movies
typedef struct {
    Movie* table[TABLE_SIZE];
    int totalMovies;
    int totalRatings;
} MovieDB;

// System Metadata
typedef struct {
    char availableGenres[MAX_GENRES][MAX_GENRE];
    int genreCount;
    int nextMovieId;
    int totalUsers;
} SystemConfig;

SystemConfig config;

// ========== UTILITY FUNCTIONS ==========

void toTitleCase(char* str) {
    int newWord = 1;
    for (int i = 0; str[i]; i++) {
        if (newWord && str[i] != ' ') {
            str[i] = toupper(str[i]);
            newWord = 0;
        } else if (str[i] == ' ') {
            newWord = 1;
        } else {
            str[i] = tolower(str[i]);
        }
    }
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

unsigned int hashFunction(const char* title) {
    unsigned int hash = 5381;
    int c;
    while ((c = *title++)) {
        hash = ((hash << 5) + hash) + tolower(c);
    }
    return hash % TABLE_SIZE;
}

// ========== SYSTEM INITIALIZATION ==========

void initSystem() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                   ║\n");
    printf("║          🎬 MOVIE RATING & RECOMMENDATION SYSTEM 🎬               ║\n");
    printf("║       Using Hash Tables, Linked Lists & Sorting Algorithms        ║\n");
    printf("║                                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("                     SYSTEM INITIALIZATION\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    // Initialize default genres
    strcpy(config.availableGenres[0], "ACTION");
    strcpy(config.availableGenres[1], "COMEDY");
    strcpy(config.availableGenres[2], "DRAMA");
    strcpy(config.availableGenres[3], "HORROR");
    strcpy(config.availableGenres[4], "SCI-FI");
    strcpy(config.availableGenres[5], "ROMANCE");
    strcpy(config.availableGenres[6], "THRILLER");
    strcpy(config.availableGenres[7], "ANIMATION");
    config.genreCount = 8;
    config.nextMovieId = 1001;
    config.totalUsers = 0;
    
    printf("✓ System initialized successfully!\n");
    printf("✓ Database ready with Hash Table (Size: %d)\n", TABLE_SIZE);
    printf("✓ Available Genres: %d\n\n", config.genreCount);
    
    printf("📋 Supported Genres:\n");
    for (int i = 0; i < config.genreCount; i++) {
        printf("   %d. %s\n", i + 1, config.availableGenres[i]);
    }
    printf("\n");
}

void initDB(MovieDB* db) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        db->table[i] = NULL;
    }
    db->totalMovies = 0;
    db->totalRatings = 0;
}

// ========== MOVIE OPERATIONS ==========

void addMovie(MovieDB* db, const char* title, const char* genre, int year) {
    // Validate genre
    int validGenre = 0;
    char upperGenre[MAX_GENRE];
    strcpy(upperGenre, genre);
    for (int i = 0; upperGenre[i]; i++) upperGenre[i] = toupper(upperGenre[i]);
    
    for (int i = 0; i < config.genreCount; i++) {
        if (strcmp(upperGenre, config.availableGenres[i]) == 0) {
            validGenre = 1;
            break;
        }
    }
    
    if (!validGenre) {
        printf("\n❌ ERROR: Invalid genre '%s'\n", genre);
        printf("💡 Available genres:\n");
        for (int i = 0; i < config.genreCount; i++) {
            printf("   • %s\n", config.availableGenres[i]);
        }
        return;
    }
    
    unsigned int index = hashFunction(title);
    
    // Check for duplicate
    Movie* temp = db->table[index];
    while (temp != NULL) {
        if (strcasecmp(temp->title, title) == 0) {
            printf("\n❌ ERROR: Movie '%s' already exists!\n", title);
            return;
        }
        temp = temp->next;
    }
    
    // Create new movie
    Movie* newMovie = (Movie*)malloc(sizeof(Movie));
    if (!newMovie) {
        printf("❌ Memory allocation failed!\n");
        return;
    }
    
    newMovie->id = config.nextMovieId++;
    strcpy(newMovie->title, title);
    toTitleCase(newMovie->title);
    strcpy(newMovie->genre, upperGenre);
    newMovie->year = year;
    newMovie->ratingSum = 0.0;
    newMovie->ratingCount = 0;
    newMovie->ratings = NULL;
    newMovie->next = db->table[index];
    db->table[index] = newMovie;
    db->totalMovies++;
    
    printf("\n✅ SUCCESS: Movie added to database!\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│  Movie ID   : %-49d│\n", newMovie->id);
    printf("│  Title      : %-49s│\n", newMovie->title);
    printf("│  Genre      : %-49s│\n", newMovie->genre);
    printf("│  Year       : %-49d│\n", newMovie->year);
    printf("│  Hash Index : %-49d│\n", index);
    printf("└─────────────────────────────────────────────────────────────────┘\n");
}

Movie* searchMovieByTitle(MovieDB* db, const char* title) {
    unsigned int index = hashFunction(title);
    Movie* temp = db->table[index];
    
    while (temp != NULL) {
        if (strcasecmp(temp->title, title) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    
    return NULL;
}

Movie* searchMovieById(MovieDB* db, int id) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie* temp = db->table[i];
        while (temp != NULL) {
            if (temp->id == id) {
                return temp;
            }
            temp = temp->next;
        }
    }
    return NULL;
}

void rateMovie(MovieDB* db, const char* title, int userId, int rating) {
    if (rating < 1 || rating > 5) {
        printf("\n❌ ERROR: Rating must be between 1-5 stars!\n");
        return;
    }
    
    Movie* movie = searchMovieByTitle(db, title);
    if (!movie) {
        printf("\n❌ ERROR: Movie '%s' not found!\n", title);
        printf("💡 Tip: Use 'Search Movie' to find exact title\n");
        return;
    }
    
    // Check if user already rated
    Rating* ratingNode = movie->ratings;
    while (ratingNode != NULL) {
        if (ratingNode->userId == userId) {
            // Update existing rating
            movie->ratingSum = movie->ratingSum - ratingNode->rating + rating;
            ratingNode->rating = rating;
            printf("\n✅ Rating updated successfully!\n");
            printf("┌─────────────────────────────────────────────────────────────────┐\n");
            printf("│  Movie      : %-49s│\n", movie->title);
            printf("│  Your Rating: %-49d★\n", rating);
            printf("│  Avg Rating : %-48.2f★\n", movie->ratingSum / movie->ratingCount);
            printf("└─────────────────────────────────────────────────────────────────┘\n");
            return;
        }
        ratingNode = ratingNode->next;
    }
    
    // Add new rating
    Rating* newRating = (Rating*)malloc(sizeof(Rating));
    newRating->userId = userId;
    newRating->rating = rating;
    newRating->next = movie->ratings;
    movie->ratings = newRating;
    
    movie->ratingSum += rating;
    movie->ratingCount++;
    db->totalRatings++;
    
    printf("\n✅ SUCCESS: Rating added!\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│  Movie      : %-49s│\n", movie->title);
    printf("│  Your Rating: %-49d★\n", rating);
    printf("│  Avg Rating : %-48.2f★\n", movie->ratingSum / movie->ratingCount);
    printf("│  Total Votes: %-49d│\n", movie->ratingCount);
    printf("└─────────────────────────────────────────────────────────────────┘\n");
}

// ========== DISPLAY FUNCTIONS ==========

void displayMovie(Movie* movie) {
    float avgRating = (movie->ratingCount > 0) ? 
                      (movie->ratingSum / movie->ratingCount) : 0.0;
    
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│  ID    : %-54d│\n", movie->id);
    printf("│  Title : %-54s│\n", movie->title);
    printf("│  Genre : %-54s│\n", movie->genre);
    printf("│  Year  : %-54d│\n", movie->year);
    printf("│  Rating: ⭐ %.2f/5.00 (%d votes)%-29s│\n", 
           avgRating, movie->ratingCount, "");
    printf("└─────────────────────────────────────────────────────────────────┘\n");
}

void displayAllMovies(MovieDB* db) {
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    MOVIE DATABASE - ALL MOVIES                    ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════╣\n");
    printf("║  Total Movies: %-51d║\n", db->totalMovies);
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    if (db->totalMovies == 0) {
        printf("⚠ Database is empty! Add movies first.\n");
        return;
    }
    
    printf("┌──────┬──────────────────────────────┬─────────────┬──────┬─────────┐\n");
    printf("│  ID  │           TITLE              │    GENRE    │ YEAR │ RATING  │\n");
    printf("├──────┼──────────────────────────────┼─────────────┼──────┼─────────┤\n");
    
    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie* temp = db->table[i];
        while (temp != NULL) {
            float avgRating = (temp->ratingCount > 0) ? 
                             (temp->ratingSum / temp->ratingCount) : 0.0;
            printf("│ %-5d│ %-29s│ %-12s│ %-5d│ ⭐%-5.2f│\n",
                   temp->id, temp->title, temp->genre, temp->year, avgRating);
            temp = temp->next;
        }
    }
    
    printf("└──────┴──────────────────────────────┴─────────────┴──────┴─────────┘\n");
}

void searchByGenre(MovieDB* db, const char* genre) {
    char upperGenre[MAX_GENRE];
    strcpy(upperGenre, genre);
    for (int i = 0; upperGenre[i]; i++) upperGenre[i] = toupper(upperGenre[i]);
    
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║            MOVIES IN '%s' GENRE", upperGenre);
    int padding = 52 - strlen(upperGenre);
    for (int i = 0; i < padding; i++) printf(" ");
    printf("║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    int found = 0;
    
    printf("┌──────┬──────────────────────────────┬──────┬─────────┬───────────┐\n");
    printf("│  ID  │           TITLE              │ YEAR │ RATING  │   VOTES   │\n");
    printf("├──────┼──────────────────────────────┼──────┼─────────┼───────────┤\n");
    
    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie* temp = db->table[i];
        while (temp != NULL) {
            if (strcasecmp(temp->genre, upperGenre) == 0) {
                float avgRating = (temp->ratingCount > 0) ? 
                                 (temp->ratingSum / temp->ratingCount) : 0.0;
                printf("│ %-5d│ %-29s│ %-5d│ ⭐%-5.2f│    %-7d│\n",
                       temp->id, temp->title, temp->year, avgRating, temp->ratingCount);
                found++;
            }
            temp = temp->next;
        }
    }
    
    printf("└──────┴──────────────────────────────┴──────┴─────────┴───────────┘\n");
    
    if (found == 0) {
        printf("\n⚠ No movies found in '%s' genre.\n", upperGenre);
    } else {
        printf("\n📊 Total: %d movie(s)\n", found);
    }
}

// ========== SORTING & RECOMMENDATION ==========

Movie** collectMovies(MovieDB* db, int* size) {
    *size = db->totalMovies;
    if (*size == 0) return NULL;
    
    Movie** arr = (Movie**)malloc(sizeof(Movie*) * (*size));
    int idx = 0;
    
    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie* temp = db->table[i];
        while (temp != NULL) {
            arr[idx++] = temp;
            temp = temp->next;
        }
    }
    
    return arr;
}

void sortByRating(Movie** arr, int n) {
    // Bubble Sort (descending order)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            float avg1 = (arr[j]->ratingCount > 0) ? 
                        (arr[j]->ratingSum / arr[j]->ratingCount) : 0.0;
            float avg2 = (arr[j + 1]->ratingCount > 0) ? 
                        (arr[j + 1]->ratingSum / arr[j + 1]->ratingCount) : 0.0;
            
            if (avg1 < avg2) {
                Movie* temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void sortByTitle(Movie** arr, int n) {
    // Bubble Sort (alphabetical)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (strcasecmp(arr[j]->title, arr[j + 1]->title) > 0) {
                Movie* temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void displaySortedMovies(Movie** arr, int n, const char* sortBy) {
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    SORTED MOVIE LIST                              ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════╣\n");
    printf("║  Sorted By: %-54s║\n", sortBy);
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("┌──────┬──────┬──────────────────────────────┬─────────────┬─────────┐\n");
    printf("│ RANK │  ID  │           TITLE              │    GENRE    │ RATING  │\n");
    printf("├──────┼──────┼──────────────────────────────┼─────────────┼─────────┤\n");
    
    for (int i = 0; i < n; i++) {
        float avgRating = (arr[i]->ratingCount > 0) ? 
                         (arr[i]->ratingSum / arr[i]->ratingCount) : 0.0;
        printf("│  %-4d│ %-5d│ %-29s│ %-12s│ ⭐%-5.2f│\n",
               i + 1, arr[i]->id, arr[i]->title, arr[i]->genre, avgRating);
    }
    
    printf("└──────┴──────┴──────────────────────────────┴─────────────┴─────────┘\n");
}

void recommendTopMovies(MovieDB* db, int topN) {
    int size;
    Movie** movies = collectMovies(db, &size);
    
    if (!movies || size == 0) {
        printf("\n⚠ No movies available for recommendation!\n");
        return;
    }
    
    sortByRating(movies, size);
    
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                 🌟 TOP %d RECOMMENDED MOVIES 🌟                    ║\n", topN);
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    int count = (topN < size) ? topN : size;
    
    for (int i = 0; i < count; i++) {
        if (movies[i]->ratingCount == 0) continue;  // Skip unrated
        
        float avgRating = movies[i]->ratingSum / movies[i]->ratingCount;
        printf("🏆 Rank %d: %s\n", i + 1, movies[i]->title);
        printf("   Genre: %s | Year: %d | Rating: ⭐%.2f/5.00 (%d votes)\n",
               movies[i]->genre, movies[i]->year, avgRating, movies[i]->ratingCount);
        printf("   ─────────────────────────────────────────────────────────────\n");
    }
    
    free(movies);
}

// ========== ANALYTICS ==========

void generateAnalytics(MovieDB* db) {
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                  MOVIE DATABASE ANALYTICS                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    // Genre statistics
    int genreCount[MAX_GENRES] = {0};
    float genreRatings[MAX_GENRES] = {0};
    
    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie* temp = db->table[i];
        while (temp != NULL) {
            for (int j = 0; j < config.genreCount; j++) {
                if (strcmp(temp->genre, config.availableGenres[j]) == 0) {
                    genreCount[j]++;
                    if (temp->ratingCount > 0) {
                        genreRatings[j] += temp->ratingSum / temp->ratingCount;
                    }
                    break;
                }
            }
            temp = temp->next;
        }
    }
    
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│  DATABASE STATISTICS                                            │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│  Total Movies            : %-33d│\n", db->totalMovies);
    printf("│  Total Ratings Given     : %-33d│\n", db->totalRatings);
    printf("│  Average Ratings/Movie   : %-32.2f│\n", 
           (db->totalMovies > 0) ? (float)db->totalRatings / db->totalMovies : 0);
    printf("└─────────────────────────────────────────────────────────────────┘\n\n");
    
    printf("┌───────────────────┬──────────┬───────────────┐\n");
    printf("│      GENRE        │  MOVIES  │  AVG RATING   │\n");
    printf("├───────────────────┼──────────┼───────────────┤\n");
    
    for (int i = 0; i < config.genreCount; i++) {
        float avgRating = (genreCount[i] > 0) ? genreRatings[i] / genreCount[i] : 0.0;
        printf("│ %-18s│    %-6d│   ⭐%-8.2f│\n",
               config.availableGenres[i], genreCount[i], avgRating);
    }
    
    printf("└───────────────────┴──────────┴───────────────┘\n");
}

void displayHashTableStructure(MovieDB* db) {
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║              HASH TABLE STRUCTURE (Internal View)                 ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════╣\n");
    printf("║  Shows collision chains and hash distribution                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    int maxChain = 0, emptyBuckets = 0;
    
    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie* temp = db->table[i];
        int chainLen = 0;
        
        if (temp == NULL) {
            if (i < 10) {
                printf("Index [%d ]: ∅\n", i);
            } else {
                printf("Index [%d]: ∅\n", i);
            }
            emptyBuckets++;
        } else {
            if (i < 10) {
                printf("Index [%d ]: ", i);
            } else {
                printf("Index [%d]: ", i);
            }
            while (temp != NULL) {
                printf("→ [%s]", temp->title);
                temp = temp->next;
                chainLen++;
            }
            printf(" → NULL\n");
            
            if (chainLen > maxChain) maxChain = chainLen;
        }
    }
    
    printf("\n📊 Hash Table Statistics:\n");
    printf("  • Total Buckets     : %d\n", TABLE_SIZE);
    printf("  • Used Buckets      : %d\n", TABLE_SIZE - emptyBuckets);
    printf("  • Empty Buckets     : %d\n", emptyBuckets);
    printf("  • Max Chain Length  : %d\n", maxChain);
    printf("  • Load Factor       : %.2f%%\n", (db->totalMovies * 100.0) / TABLE_SIZE);
    printf("  • Avg Chain Length  : %.2f\n", (float)db->totalMovies / (TABLE_SIZE - emptyBuckets));
}

// ========== MENU ==========

void displayMenu() {
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║              MOVIE RECOMMENDATION SYSTEM - MAIN MENU              ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════╣\n");
    printf("║  1. ➕ Add New Movie                                              ║\n");
    printf("║  2. ⭐ Rate a Movie                                               ║\n");
    printf("║  3. 🔍 Search Movie by Title                                      ║\n");
    printf("║  4. 🔎 Search Movies by Genre                                     ║\n");
    printf("║  5. 📋 Display All Movies                                         ║\n");
    printf("║  6. 📊 Sort Movies by Rating                                      ║\n");
    printf("║  7. 🔤 Sort Movies by Title                                       ║\n");
    printf("║  8. 🌟 Get Top Movie Recommendations                              ║\n");
    printf("║  9. 📈 View Database Analytics                                    ║\n");
    printf("║ 10. 🔢 View Hash Table Structure                                  ║\n");
    printf("║ 11. ℹ️  View Complexity Analysis                                  ║\n");
    printf("║ 12. 🚪 Exit System                                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf("\nEnter your choice (1-12): ");
}

void freeDB(MovieDB* db) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie* movie = db->table[i];
        while (movie != NULL) {
            // Free ratings
            Rating* rating = movie->ratings;
            while (rating != NULL) {
                Rating* nextRating = rating->next;
                free(rating);
                rating = nextRating;
            }
            
            Movie* nextMovie = movie->next;
            free(movie);
            movie = nextMovie;
        }
    }
}

// ========== MAIN ==========

int main() {
    MovieDB db;
    initDB(&db);
    initSystem();
    
    int choice, userId = 1;  // Simple user ID for demo
    
    while (1) {
        displayMenu();
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch (choice) {
            case 1: {
                char title[MAX_TITLE], genre[MAX_GENRE];
                int year;
                
                printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                printf("                       ADD NEW MOVIE\n");
                printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
                
                printf("🎬 Movie Title: ");
                fgets(title, MAX_TITLE, stdin);
                title[strcspn(title, "\n")] = 0;
                
                printf("📂 Genre (");
                for (int i = 0; i < config.genreCount; i++) {
                    printf("%s", config.availableGenres[i]);
                    if (i < config.genreCount - 1) printf(", ");
                }
                printf("): ");
                fgets(genre, MAX_GENRE, stdin);
                genre[strcspn(genre, "\n")] = 0;
                
                printf("📅 Release Year: ");
                scanf("%d", &year);
                
                addMovie(&db, title, genre, year);
                break;
            }
            
            case 2: {
                char title[MAX_TITLE];
                int rating;
                
                printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                printf("                       RATE A MOVIE\n");
                printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
                
                printf("🎬 Movie Title: ");
                fgets(title, MAX_TITLE, stdin);
                title[strcspn(title, "\n")] = 0;
                
                printf("⭐ Your Rating (1-5 stars): ");
                scanf("%d", &rating);
                
                rateMovie(&db, title, userId, rating);
                break;
            }
            
            case 3: {
                char title[MAX_TITLE];
                
                printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                printf("                   SEARCH MOVIE BY TITLE\n");
                printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
                
                printf("🔍 Enter Movie Title: ");
                fgets(title, MAX_TITLE, stdin);
                title[strcspn(title, "\n")] = 0;
                
                Movie* movie = searchMovieByTitle(&db, title);
                if (movie) {
                    printf("\n✅ Movie found!\n");
                    displayMovie(movie);
                } else {
                    printf("\n❌ Movie '%s' not found in database!\n", title);
                }
                break;
            }
            
            case 4: {
                char genre[MAX_GENRE];
                
                printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                printf("                  SEARCH MOVIES BY GENRE\n");
                printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
                
                printf("📂 Available Genres:\n");
                for (int i = 0; i < config.genreCount; i++) {
                    printf("   %d. %s\n", i + 1, config.availableGenres[i]);
                }
                
                printf("\n🔎 Enter Genre: ");
                fgets(genre, MAX_GENRE, stdin);
                genre[strcspn(genre, "\n")] = 0;
                
                searchByGenre(&db, genre);
                break;
            }
            
            case 5:
                displayAllMovies(&db);
                break;
            
            case 6: {
                int size;
                Movie** movies = collectMovies(&db, &size);
                if (movies) {
                    sortByRating(movies, size);
                    displaySortedMovies(movies, size, "RATING (Highest to Lowest)");
                    printf("\n⏱  Time Complexity: O(n²) - Bubble Sort Algorithm\n");
                    free(movies);
                } else {
                    printf("\n⚠ No movies to sort!\n");
                }
                break;
            }
            
            case 7: {
                int size;
                Movie** movies = collectMovies(&db, &size);
                if (movies) {
                    sortByTitle(movies, size);
                    displaySortedMovies(movies, size, "TITLE (Alphabetical Order)");
                    printf("\n⏱  Time Complexity: O(n²) - Bubble Sort Algorithm\n");
                    free(movies);
                } else {
                    printf("\n⚠ No movies to sort!\n");
                }
                break;
            }
            
            case 8: {
                int topN;
                printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                printf("                  TOP MOVIE RECOMMENDATIONS\n");
                printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
                
                printf("🌟 How many top movies to display? (1-10): ");
                scanf("%d", &topN);
                
                if (topN < 1 || topN > 10) topN = 5;
                
                recommendTopMovies(&db, topN);
                break;
            }
            
            case 9:
                generateAnalytics(&db);
                break;
            
            case 10:
                displayHashTableStructure(&db);
                break;
            
            case 11:
                printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
                printf("║              TIME & SPACE COMPLEXITY ANALYSIS                     ║\n");
                printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
                
                printf("┌─────────────────────────┬─────────────────┬─────────────────┐\n");
                printf("│       OPERATION         │  AVERAGE CASE   │   WORST CASE    │\n");
                printf("├─────────────────────────┼─────────────────┼─────────────────┤\n");
                printf("│ Add Movie               │      O(1)       │      O(n)       │\n");
                printf("│ Search by Title (Hash)  │      O(1)       │      O(n)       │\n");
                printf("│ Search by Genre         │      O(n)       │      O(n)       │\n");
                printf("│ Rate Movie              │      O(1)       │      O(n)       │\n");
                printf("│ Sort by Rating          │      O(n²)      │      O(n²)      │\n");
                printf("│ Sort by Title           │      O(n²)      │      O(n²)      │\n");
                printf("│ Top Recommendations     │      O(n²)      │      O(n²)      │\n");
                printf("│ Display All             │      O(n)       │      O(n)       │\n");
                printf("│ Generate Analytics      │      O(n)       │      O(n)       │\n");
                printf("└─────────────────────────┴─────────────────┴─────────────────┘\n");
                
                printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
                printf("│  SPACE COMPLEXITY ANALYSIS                                      │\n");
                printf("├─────────────────────────────────────────────────────────────────┤\n");
                printf("│  Hash Table          : O(%d) = O(1) constant space           │\n", TABLE_SIZE);
                printf("│  Movie Records       : O(m) where m = number of movies         │\n");
                printf("│  Rating Records      : O(r) where r = total ratings            │\n");
                printf("│  Chaining (Linked)   : O(m) for collision resolution           │\n");
                printf("│  Total Space         : O(m + r)                                 │\n");
                printf("└─────────────────────────────────────────────────────────────────┘\n");
                
                printf("\n💡 OPTIMIZATION NOTES:\n");
                printf("  • Hash-based search gives O(1) average lookup time\n");
                printf("  • Collision handling via chaining (linked lists)\n");
                printf("  • Worst case O(n) when all movies hash to same index\n");
                printf("  • Can improve sort to O(n log n) with Merge/Quick Sort\n");
                printf("  • Load factor: %.2f%% (optimal is < 75%%)\n", 
                       (db.totalMovies * 100.0) / TABLE_SIZE);
                
                if ((db.totalMovies * 100.0) / TABLE_SIZE > 75) {
                    printf("\n⚠ WARNING: High load factor detected!\n");
                    printf("  Consider increasing TABLE_SIZE for better performance.\n");
                }
                break;
            
            case 12:
                printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
                printf("║                    SYSTEM SHUTDOWN                                ║\n");
                printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
                
                printf("📊 Final Statistics:\n");
                printf("  • Total Movies Added    : %d\n", db.totalMovies);
                printf("  • Total Ratings Given   : %d\n", db.totalRatings);
                if (db.totalMovies > 0) {
                    printf("  • Avg Ratings per Movie : %.2f\n", 
                           (float)db.totalRatings / db.totalMovies);
                }
                
                printf("\n🧹 Cleaning up database memory...\n");
                freeDB(&db);
                printf("✅ Memory freed successfully!\n");
                
                printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
                printf("║      Thank you for using Movie Recommendation System!            ║\n");
                printf("║               🎬 Happy Movie Watching! 🎬                         ║\n");
                printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
                
                return 0;
            
            default:
                printf("\n❌ Invalid choice! Please enter a number between 1-12.\n");
        }
        
        printf("\n⏸  Press Enter to continue...");
        getchar();
    }
    
    return 0;
}
