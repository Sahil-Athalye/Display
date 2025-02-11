1. CSE 542 Fall 2024 Lab 3

2. 
Sahil Athalye: a.sahil@wustl.edu
Varad Deouskar: varad@wustl.edu

3. We developed our program from simple single-threaded file IO into a multi-threaded, networked solution using shared concurrency primitives, careful error handling, and a unified interface for both local and remote resources.
To support concurrency, we replaced direct data storage in structs with Arc<Mutex<...>>, allowing multiple threads to safely share and modify data. We now lock these mutexes before accessing SceneFragment or Player data, ensuring thread safety. When sorting, we implemented a custom comparison function that locks and compares the underlying Player data.
For output, we switched from println! and eprintln! to writeln! with stdout().lock() and stderr().lock(), ensuring that even if multiple threads print simultaneously, the output remains thread-safe and consistent.
We extended the IO model so our code can handle both local file operations and networked file requests. If a token matches the pattern net:address:port:filename, we connect to a server, send the filename, and receive its contents as if it were a local file.
On the server side, we run a loop accepting connections and spawning threads. A global CANCEL_FLAG signals when to stop. Each connection thread reads tokens, sends file contents, or sets the cancel flag for "quit." We ensure no directory traversal or environment expansion occurs by validating tokens.
On the client side, we can send tokens, receive file contents, or trigger the server to shut down. Errors and panics in worker threads are propagated to the main thread through join, preserving error handling behavior.

4.

*****SECTION 2: HOW TO RUN PROGRAM*****
1. HOW TO UNZIP/UNPACK FILES: Unzip the .zip file and all the provided files will be in the lab3 folder. 
2. HOW TO BUILD PROGRAM: Once inside the lab3 folder,  there will be packages for the client and server. “cargo build" will build the executable from the source code when within both the client and server. The executable will then be found in the debug folder of the target folder for each package. 
3. HOW TO RUN PROGRAM: To run entirely locally: Once in the debug folder of the client, you will find all the required text files. Make sure all the inner config text files are in the same folder as the executable. 
Run the program as such: ./lab3client <config_file_path> <OPTIONAL: "whinge">.
To run remotely: Once in the debug folder of the server, you will find all the required text files. Make sure all the inner config text files are in the same folder as the executable. 
First start running the server with ./lab3server <port>
Then run the client with ./lab3client <config_file_path> <OPTIONAL: "whinge">
For the config_file_path, the network prefix will need to be appended e.g. if the port name is given as 127.0.0.1:7777 when running the server, use net:127.0.0.1:7777:partial_macbeth_act_i_script.txt as the config_file_path

*****SECTION 3: REFACTORING AND TESTING*****
To refactor the code, we first replaced the Player instances in the SceneFragment struct's characters vector with Arc<Mutex<Player>> to ensure thread safety through shared ownership and safe mutability. Instead of directly borrowing mutable references to Player, we now use player.lock() to acquire a lock, ensuring that only one thread can access the player at a time, which prevents data races and resolves borrow checker issues related to mutable references. In the process_config method, we modified the way new players were added by wrapping each Player in a Mutex and then an Arc before pushing it into the vector. We also updated the logic for selecting the next player in the recite method, ensuring the lock is held only long enough to update the player's line number, minimizing blocking and improving performance. Additionally, we introduced a custom comparison function to compare Arc<Mutex<Player>> references based on the underlying Player data and replaced direct sorting of the vector with the sort_by method. Overall, these changes improve the safety, correctness, and concurrency of the program by minimizing lock contention and ensuring that mutable references are handled safely.
*****TESTING*****
Testing Approach
The testing methodology was similar to lab 2.

We used the "2>" to make sure out errors are going to stderr.

Variety of Inputs: We created multiple configuration and character files to cover edge cases, including well-formed and malformed content.

For error checking we had test cases like invalid number of program args, invalid lines such as having invalid line index, invalid file names and extra whitespaces. This was done when running with files from the server and local files.

Automation: Used shell scripts to automate testing with different inputs and capture outputs for comparison.

Validation: Manually verified outputs against expected results, ensuring correct ordering and formatting.

We added a number of lines between each character name/character file line to examine correct behavior. We included additional tokens on each line of the config file. We included fewer than two tokens on lines in the config file. We spelled file names incorrectly and moved files to various locations within the file tree. We included multiple lines with the same line number across a combination of different files. We provided file names that did not exist for both the outer and inner configuration file(s). We also passed file names in as the title of the play and tested with correctly formatted inputs.

Another important case we applied was adding the network prefix to the file name of every configuration file. If the prefix was added to the file name within its outer config file, the file was searched for remotely. If there was no prefix, then the file was searched for locally. We monitored multiple combinations of prefix attachment as well as existence in the local and server file trees for correct behavior.

Observations from Testing

The program effectively handles various error conditions and warns of error when whinge is enabled. 

OUTPUT FROM SUBMITTED TEXT FILES: 
Hamlet Prince of Denmark ACT II Scene I A room in Polonius house by William Shakespeare
[Enter Polonius.]
[Enter Reynaldo.]

Polonius
Give him this money and these notes, Reynaldo.

Reynaldo
I will, my lord.

Polonius
You shall do marvellous wisely, good Reynaldo,
Before You visit him, to make inquiry
Of his behaviour.

Reynaldo
My lord, I did intend it.

Polonius
Marry, well said; very well said. Look you, sir,
Enquire me first what Danskers are in Paris;
And how, and who, what means, and where they keep,
What company, at what expense; and finding,
By this encompassment and drift of question,
That they do know my son, come you more nearer
Than your particular demands will touch it:
Take you, as 'twere, some distant knowledge of him;
As thus, 'I know his father and his friends,
And in part hi;m;--do you mark this, Reynaldo?

Reynaldo
Ay, very well, my lord.

Polonius
'And in part him;--but,' you may say, 'not well:
But if't be he I mean, he's very wild;
Addicted so and so;' and there put on him
What forgeries you please; marry, none so rank
As may dishonour him; take heed of that;
But, sir, such wanton, wild, and usual slips
As are companions noted and most known
To youth and liberty.

Reynaldo
As gaming, my lord.

Polonius
Ay, or drinking, fencing, swearing, quarrelling,
Drabbing:--you may go so far.

Reynaldo
My lord, that would dishonour him.

Polonius
Faith, no; as you may season it in the charge.
You must not put another scandal on him,
That he is open to incontinency;
That's not my meaning: but breathe his faults so quaintly
That they may seem the taints of liberty;
The flash and outbreak of a fiery mind;
A savageness in unreclaimed blood,
Of general assault.

Reynaldo
But, my good lord,--

Polonius
Wherefore should you do this?

Reynaldo
Ay, my lord,
I would know that.

Polonius
Marry, sir, here's my drift;
And I believe it is a fetch of warrant:
You laying these slight sullies on my son
As 'twere a thing a little soil'd i' the working,
Mark you,
Your party in converse, him you would sound,
Having ever seen in the prenominate crimes
The youth you breathe of guilty, be assur'd
He closes with you in this consequence;
'Good sir,' or so; or 'friend,' or 'gentleman'--
According to the phrase or the addition
Of man and country.

Reynaldo
Very good, my lord.

Polonius
And then, sir, does he this,--he does--What was I about to say?--
By the mass, I was about to say something:--Where did I leave?

Reynaldo
At 'closes in the consequence,' at 'friend or so,' and
gentleman.'

Polonius
At--closes in the consequence'--ay, marry!
He closes with you thus:--'I know the gentleman;
I saw him yesterday, or t'other day,
Or then, or then; with such, or such; and, as you say,
There was he gaming; there o'ertook in's rouse;
There falling out at tennis': or perchance,
'I saw him enter such a house of sale,'--
Videlicet, a brothel,--or so forth.--
See you now;
Your bait of falsehood takes this carp of truth:
And thus do we of wisdom and of reach,
With windlaces, and with assays of bias,
By indirections find directions out:
So, by my former lecture and advice,
Shall you my son. You have me, have you not?

Reynaldo
My lord, I have.

Polonius
God b' wi' you, fare you well.

Reynaldo
Good my lord!

Polonius
Observe his inclination in yourself.

Reynaldo
I shall, my lord.

Polonius
And let him ply his music.

Reynaldo
Well, my lord.

Polonius
Farewell!
[Exit Reynaldo.]
[Enter Ophelia.]

Polonius
How now, Ophelia! what's the matter?

Ophelia
Alas, my lord, I have been so affrighted!

Polonius
With what, i' the name of God?

Ophelia
My lord, as I was sewing in my chamber,
Lord Hamlet,--with his doublet all unbrac'd;
No hat upon his head; his stockings foul'd,
Ungart'red, and down-gyved to his ankle;
Pale as his shirt; his knees knocking each other;
And with a look so piteous in purport
As if he had been loosed out of hell
To speak of horrors,--he comes before me.

Polonius
Mad for thy love?

Ophelia
My lord, I do not know;
But truly I do fear it.

Polonius
What said he?

Ophelia
He took me by the wrist, and held me hard;
Then goes he to the length of all his arm;
And with his other hand thus o'er his brow,
He falls to such perusal of my face
As he would draw it. Long stay'd he so;
At last,--a little shaking of mine arm,
And thrice his head thus waving up and down,--
He rais'd a sigh so piteous and profound
As it did seem to shatter all his bulk
And end his being: that done, he lets me go:
And, with his head over his shoulder turn'd
He seem'd to find his way without his eyes;
For out o' doors he went without their help,
And to the last bended their light on me.

Polonius
Come, go with me: I will go seek the king.
This is the very ecstasy of love;
Whose violent property fordoes itself,
And leads the will to desperate undertakings,
As oft as any passion under heaven
That does afflict our natures. I am sorry,--
What, have you given him any hard words of late?

Ophelia
No, my good lord; but, as you did command,
I did repel his letters and denied
His access to me.

Polonius
That hath made him mad.
I am sorry that with better heed and judgment
I had not quoted him: I fear'd he did but trifle,
And meant to wreck thee; but beshrew my jealousy!
It seems it as proper to our age
To cast beyond ourselves in our opinions
As it is common for the younger sort
To lack discretion. Come, go we to the king:
This must be known; which, being kept close, might move
More grief to hide than hate to utter love.
[Exit Ophelia.]
[Exit Polonius.]
Hamlet Prince of Denmark ACT II Scene II A room in the Castle by William Shakespeare
[Enter King.]
[Enter Queen.]
[Enter Rosencrantz.]
[Enter Guildenstern.]

King
Welcome, dear Rosencrantz and Guildenstern!
Moreover that we much did long to see you,
The need we have to use you did provoke
Our hasty sending. Something have you heard
Of Hamlet's transformation; so I call it,
Since nor the exterior nor the inward man
Resembles that it was. What it should be,
More than his father's death, that thus hath put him
So much from the understanding of himself,
I cannot dream of: I entreat you both
That, being of so young days brought up with him,
And since so neighbour'd to his youth and humour,
That you vouchsafe your rest here in our court
Some little time: so by your companies
To draw him on to pleasures, and to gather,
So much as from occasion you may glean,
Whether aught, to us unknown, afflicts him thus,
That, open'd, lies within our remedy.

Queen
Good gentlemen, he hath much talk'd of you,
And sure I am two men there are not living
To whom he more adheres. If it will please you
To show us so much gentry and good-will
As to expend your time with us awhile,
For the supply and profit of our hope,
Your visitation shall receive such thanks
As fits a king's remembrance.

Rosencrantz
Both your majesties
Might, by the sovereign power you have of us,
Put your dread pleasures more into command
Than to entreaty.

Guildenstern
We both obey,
And here give up ourselves, in the full bent,
To lay our service freely at your feet,
To be commanded.

King
Thanks, Rosencrantz and gentle Guildenstern.

Queen
Thanks, Guildenstern and gentle Rosencrantz:
And I beseech you instantly to visit
My too-much-changed son.--Go, some of you,
And bring these gentlemen where Hamlet is.

Guildenstern
Heavens make our presence and our practices
Pleasant and helpful to him!

Queen
Ay, amen!
[Exit Guildenstern.]
[Exit Rosencrantz.]
[Exit Queen.]
[Exit King.]

