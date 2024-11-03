# Communication System

Contributor: Jaicharan Yeluri

My work was to build an effective communication system for our college.
However, Implementing the entire communication
system from scratch would have been a lengthy process, making it difficult to complete the project within a feasible
timeframe.
To address this, I used an existing project of my team member, Agnirudra.
His project, a Discord messenger
clone, closely resembles the communication system we envisioned for our college.
By building upon this foundation, we
were able to streamline development and bring our idea closer to realization.

The aim of this project is to establish a single communication system for the entire college, allowing for streamlined,
effective communication with all users, with minimal effort.

## Design

In the project, I worked on how to process multiple messages when several users are posting messages in the same group.
Several users can post messages in the same group at the same time. But we need to reliably display these messages to
all clients in the order that they were processed on the server. To achieve this, I implemented a queue data structure
using a linked list. Both the linked list and the queue data structure are implemented in Python for easy integration
with the existing project.

## Implementation

I have implemented linked list and the queue data structure for searching in python in the `linked_list.py` and
`queue.py` files respectively.
Linked lists were used over dynamic arrays because they allow for constant-time
insertions, i.e., $O(1)$ time complexity for deletion at head, and $O(1)$ for insertion at the end,
if a pointer to the end
of the list is maintained.
This is crucial for our project, as we need to process messages quickly and efficiently.

### Github links to the documented codes:

- **Linked
  lists
  **: [](https://github.com/JCode16/avault/blob/ff7407cd5df86d551fe315a41cd330943d97bdd7/backend/app/api/core/linked_list.py)
- **Queue
  **: [](https://github.com/JCode16/avault/blob/092398f92ebcbae0a097a244a56606b39508bbf4/backend/app/api/core/queue.py)

## Execution

Let us take a realistic situation, Suppose I have a group chat with my team members.

![Group Chat](before_sending.png)

We would be sending different messages at different time intervals. The feature that I implemented now would take all
these messages based on a FIFO(first in first out) principle and dequeue them in the chat group

![Group Chat](immediately-visible.png)
The user who is viewing the channel will be able to see the message immediately after it is sent.

![Group Chat](notified.png)
The user will also be notified of the new message in the chat group through a white dot on the chat group icon.

