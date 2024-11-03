# Communication System
Contributor: Jaicharan Yeluri

My work was to build an effective communication system for our college. However, Implementing the entire communication system from scratch would have been a lengthy process, making it difficult to complete the project within a feasible timeframe. To address this, I used an existing project of my team member, Agnirudra. His project, a Discord messenger clone, closely resembles the communication system we envisioned for our college. By building upon this foundation, we were able to streamline development and bring our idea closer to realization.

The aim of this project is to establish a single communication system for the entire college, allowing for streamlined, effective communication with all users, with minimal effort.

## Design
In the project, I worked on how to process multiple messages when several users are posting messages in the same group. So my work was to orderly collect all the messages and present them in the group is a structured manner. I implemented this feature using a linear queue data structure with a singly linked list. While an array could have been used, it would make dequeuing a given node much more complex and time consuming. If the head element needs to be removed in an array, it requires shifting the indices of all subsequent elements, which would impact performance. By using a linked list, however, this challenge is minimized, as enqueuing and dequeuing a node is straightforward and efficient."

## Implementation
I have implemented linked list and the queue data structure for searching in python in the "linked_list.py" and "queue.py" files respectively.
One of the reason why I chose python over other languages to implement the data structures is because python is very direct, almost like writing in english. This way I can focus more on the logic than the syntax of the code.
I have also documented my code effectively. Links to the code are provided below

### Github links to the documented codes:

- Linked lists : https://github.com/JCode16/avault/blob/ff7407cd5df86d551fe315a41cd330943d97bdd7/backend/app/api/core/linked_list.py
- Queue : https://github.com/JCode16/avault/blob/092398f92ebcbae0a097a244a56606b39508bbf4/backend/app/api/core/queue.py




## Execution

Let us take a realistic situation, Suppose I have a group chat with my team members.

<img alt="place holder.png" src="place_holder.png"/>

we would be sending different messages at different time intervals. The feature that I implemented now would take all this messages based on a FIFO(first in first out) principle and deque them in the chat group

<img alt="place holder.png" src="place_holder.png"/>
