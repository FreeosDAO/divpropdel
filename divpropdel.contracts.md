<h1 class="contract">
   version  (TEST)
</h1>
**PARAMETERS:**

* __none__ 

**INTENT:** return version number and break the contract 

**TERM:**  TESTING ONLY (remove a.s.a.p.)

<h1 class="contract">
   dropmessage
</h1>

**PARAMETERS:**

* __proposer__ proposer name (for authorization)
* __second_voter__ the pre-defined proposer's message will be targeted only to that voter

**INTENT:** The 'proposer' message will show up for the voter with 'second_voter' name only.
Not to anybody else. 

**TERM:**  N/A

<h1 class="contract">
   div2ndvote
</h1>

**PARAMETERS:**

* __voter__ voter name (for authorization) 

**INTENT:** The intent of [[div2ndvote]] is to run a whole process of rejecting a proposal using second_voter's vote. 
The actions makd dividend vote for the given voter with pre-defined reject proposal value.
Next, the trigger table is reset to prevent re-appearing message from proposer.

**TERM:** N/A

<h1 class="contract">
   remove
</h1>

**PARAMETERS:** 

* __NONE__

**INTENT:** The intent of [[remove]] action is to reset the trigger table manually. 

**TERM:** NONE
