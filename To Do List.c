#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 100

struct Task {
    char task[100];
    char priority[10];
    char deadline[20];
    int completed;
    int index;  
    struct Task *next;
};

struct ToDoList {
    struct Task *head;
    int count;
};

void initializeList(struct ToDoList *list) {
    list->head = NULL;
    list->count = 0;
}

int isDeadlineNear(char deadline[]) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int current_day = tm.tm_mday;
    int current_month = tm.tm_mon + 1;
    int current_year = tm.tm_year + 1900;

    int day, month, year;
    sscanf(deadline, "%d-%d-%d", &day, &month, &year);

    if (year == current_year) {
        if (month == current_month) {
            if (day - current_day < 3) {
                return 1;
            }
        }
    }
    return 0;
}

void increasePriorityIfNearDeadline(struct ToDoList *list) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int current_day = tm.tm_mday;
    int current_month = tm.tm_mon + 1;
    int current_year = tm.tm_year + 1900;

    struct Task *current = list->head;
    while (current != NULL) {
        int day, month, year;
        sscanf(current->deadline, "%d-%d-%d", &day, &month, &year);

        if (year < current_year || (year == current_year && month < current_month) || (year == current_year && month == current_month && day < current_day)) {
            strcpy(current->priority, "high");
            strcpy(current->deadline, "Overdue");
        } else if (isDeadlineNear(current->deadline)) {
            if (strcmp(current->priority, "low") == 0) {
                if (day - current_day == 2) {
                    strcpy(current->priority, "medium");
                } else if (day - current_day == 1) {
                    strcpy(current->priority, "high");
                }
            } else if (strcmp(current->priority, "medium") == 0) {
                if (day - current_day == 1) {
                    strcpy(current->priority, "high");
                }
            }
        }
        current = current->next;
    }
}

void addTask(struct ToDoList *list, char task[], char priority[], char deadline[]) {
    if (list->count < MAX_SIZE) {
        struct Task *newTask = (struct Task *)malloc(sizeof(struct Task));
        strcpy(newTask->task, task);
        strcpy(newTask->priority, priority);
        strcpy(newTask->deadline, deadline);
        newTask->completed = 0;
        newTask->next = NULL;

        if (list->head == NULL || strcmp(list->head->priority, priority) < 0) {
            newTask->next = list->head;
            list->head = newTask;
        } else {
            struct Task *current = list->head;
            while (current->next != NULL && strcmp(current->next->priority, priority) >= 0) {
                current = current->next;
            }
            newTask->next = current->next;
            current->next = newTask;
        }
        list->count++;
        printf("Task added successfully.\n");
    } else {
        printf("List is full. Cannot add more tasks.\n");
    }
}

void displayTasks(struct ToDoList *list) {
    if (list->count == 0) {
        printf("No tasks in the list.\n");
    } else {
        printf("----------------------------------------------------------------------------------\n");
        printf("| Index |        Tasks                      | Priority | Deadline       | Status |\n");
        printf("----------------------------------------------------------------------------------\n");

        int counter = 1;
        struct Task *current;

        current = list->head;
        while (current != NULL) {
            if (strcmp(current->priority, "high") == 0) {
                printf("| %-5d | %-30s | %-8s | %-15s | %-6s |\n", counter, current->task, current->priority, current->deadline, current->completed ? "Done" : "Pending");
                current->index = counter;  
                counter++;
            }
            current = current->next;
        }

        current = list->head;
        while (current != NULL) {
            if (strcmp(current->priority, "medium") == 0) {
                printf("| %-5d | %-30s | %-8s | %-15s | %-6s |\n", counter, current->task, current->priority, current->deadline, current->completed ? "Done" : "Pending");
                current->index = counter;  
                counter++;
            }
            current = current->next;
        }

        current = list->head;
        while (current != NULL) {
            if (strcmp(current->priority, "low") == 0) {
                printf("| %-5d | %-30s | %-8s | %-15s | %-6s |\n", counter, current->task, current->priority, current->deadline, current->completed ? "Done" : "Pending");
                current->index = counter;  
                counter++;
            }
            current = current->next;
        }

        printf("----------------------------------------------------------------------------------\n");
    }
}

void markCompleted(struct ToDoList *list, int index) {
    struct Task *current = list->head;
    while (current != NULL) {
        if (current->index == index) {
            current->completed = 1;
            printf("Task marked as completed.\n");
            return;
        }
        current = current->next;
    }
    printf("Invalid index. Please try again.\n");
}

void editTask(struct ToDoList *list, int index, char newTaskName[], char newPriority[]) {
    struct Task *current = list->head;
    while (current != NULL) {
        if (current->index == index) {
            strcpy(current->task, newTaskName);
            strcpy(current->priority, newPriority);
            printf("Task edited successfully.\n");
            return;
        }
        current = current->next;
    }
    printf("Invalid index. Please try again.\n");
}

void deleteTask(struct ToDoList *list, int index) {
    struct Task *current = list->head;
    struct Task *prev = NULL;

    while (current != NULL) {
        if (current->index == index) {
            if (prev == NULL) {
                list->head = current->next;
                free(current);
            } else {
                prev->next = current->next;
                free(current);
            }
            list->count--;
            printf("Task deleted successfully.\n");
            return;
        } else {
            prev = current;
            current = current->next;
        }
    }
    printf("Invalid index. Please try again.\n");
}

void deleteCompletedTasks(struct ToDoList *list) {
    struct Task *current = list->head;
    struct Task *prev = NULL;

    while (current != NULL) {
        if (current->completed == 1) {
            if (prev == NULL) {
                list->head = current->next;
                free(current);
                current = list->head;
            } else {
                prev->next = current->next;
                free(current);
                current = prev->next;
            }
            list->count--;
        } else {
            prev = current;
            current = current->next;
        }
    }
    printf("All completed tasks have been deleted.\n");
}

int main() {
    struct ToDoList myList;
    initializeList(&myList);

    int choice;
    int index;
    char task[100];
    char newTaskName[100];
    char priority[10];
    char newPriority[10];
    char deadline[20];

    do {
        printf("\nTo-Do List Application\n");
        printf("1. Add Task\n");
        printf("2. Display Tasks\n");
        printf("3. Mark Task as Completed\n");
        printf("4. Edit Task\n");
        printf("5. Delete All Completed Tasks\n");
        printf("6. Delete Specified Task\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);


        switch (choice) {
            case 1:
                printf("Enter the task: ");
                scanf(" %[^\n]s", task);
                do {
                    printf("Enter the priority (low/medium/high): ");
                    scanf(" %[^\n]s", priority);
                } while (strcmp(priority, "low") != 0 && strcmp(priority, "medium") != 0 && strcmp(priority, "high") != 0);
                printf("Enter the deadline (DD-MM-YYYY): ");
                scanf(" %[^\n]s", deadline);
                addTask(&myList, task, priority, deadline);
                break;
            case 2:
                increasePriorityIfNearDeadline(&myList);
                displayTasks(&myList);
                break;
            case 3:
                displayTasks(&myList);
                printf("Enter the index of the task to mark as completed: ");
                scanf("%d", &index);
                markCompleted(&myList, index);
                break;
            case 4:
                displayTasks(&myList);
                printf("Enter the index of the task to edit: ");
                scanf("%d", &index);
                printf("Enter the new task name: ");
                scanf(" %[^\n]s", newTaskName);
                do {
                    printf("Enter the new priority (low/medium/high): ");
                    scanf(" %[^\n]s", newPriority);
                } while (strcmp(newPriority, "low") != 0 && strcmp(newPriority, "medium") != 0 && strcmp(newPriority, "high") != 0);
                editTask(&myList, index, newTaskName, newPriority);
                break;
            case 5:
                deleteCompletedTasks(&myList);
                break;
            case 6:
                printf("Enter the index of the task to delete: ");
                scanf("%d", &index);
                deleteTask(&myList, index);
                break;
            case 7:
                printf("Exiting the application. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 7);

    return 0;
}