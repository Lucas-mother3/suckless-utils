# `suckless-utils` contributions guide

Everyone can contribute to the project via pull requests, or issues. Any emails to me is acceptable, although I rarely read emails.

There are exceptions though. Any pull request or issue that is deemed controversial would be removed, not immediately (remember, I'm the guy who maintains this shit, and bear me, I am always going to school) Examples include:

- Political arguments
- Comments (inside the code and outside it) that could potentially harm, discriminate others
- Doxxing content
- Any commits that could potentially break Terms of Service or related community guidelines
- Malicious code such as miners, spyware or related. If I do push a commit that you believe have malicious code on it, please email me immediately, with a valid proof (i.e a code block or .diff patch)
- Any changes that deemed unnecessary or doesn't have a particular purpose itself

## How to write a good commit message or pull request

In this part, we would take a look on how should commits and pull requests should be written.

I do not accept these commits or pull requests:

- `Added 1 file` (which file is it?), same with `Deleted/Removed 1 file.` Be descriptive such as `Added dwm-sofia-6.4.diff` or `Removed binaries for dwm`.
- `Integrated patches`, while it is acceptable at first sight, you should add in the pull request description what patches are integrated.
- Empty pull request descriptions, at least give some description on why do you think that your changes should be integrated upsteam.

A good example of a commit message for this project would be `Replaced j4-dmenu-desktop submodule with spmenu`, which gives us a short summary of what happened, just like a headline.

A good pull request should be titled like "Integrated n patches, added features, more", with the description:
>- Added  `slstatus-midori-patch.diff` 
>- Bug fixing commit number [cda381a](https://github.com/Lucas-mother3/suckless-utils/commit/cda381a59d21db8ea8b144827029f965d25357c7)
>- Merged layouts into `layout.c`
>- General codebase improvements

## What if I don't have git?

You could submit `.diff` files into my email, or at least a tarball. It should be malware-free, or else it'll not be integrated.

Also for the love of god, use git, it's easier that way. I wasn't even bothering with reading emails nowadays. 

You could also send a tarball via Discord (at least for now as the whole source is just under 10MB), but I wouldn't recommend it.

## Suggestions, issues?

Create a new issue for suggestions or bugs, it should be descriptive, here's a general template for bug reporting:

```
Title: [MILD|NORMAL|SEVERE|CRASH] Bug in dwm causing pictures to turn white
Distribution used (Linux/BSD/Mac): openSUSE Tumbleweed 09132023
.xinitrc or similar launching script (pastebin):
Reproduction:
        1. Launch dwm via xinit
        2. Do this keybind
        3. There's the bug
Logs: 
Screenshot, if any:
Notes:
```

For writing suggestions:

```
Title: [SUGGESTION] Add dwm-elsa-6.4.diff
What does it do: Uses the libelsa library for handling window management, improving window management workflow by n% than default
Link of patch:
Why is it important: To handle windows easily than before in dwm.
```

```
Title: [SUGGESTION] Improved scaling options
Why is it important: Scaling sucks in dwm, so I was wondering if you could improve scaling for dwm. Thanks.
Codeblock for the suggestion, if able:
```

## Contact
You could email me at lucas0021a@outlook.com, also in Discord under @lucss21a, as well as in Matrix, @lucss21a:matrix.org. 
