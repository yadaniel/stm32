#!/cygdrive/c/Python39/python

import re, os, sys, time

if os.path.basename(os.getcwd()) != "FW":
    print("this script shall be called in FW directory")
    sys.exit()

patch_makefile_ = """

####################################### <PATCH #######################################
# custom make rules
# => requires CSOURCES, BUILD_DIR, TARGET
# => adds CSOURCES
# => adds checksum
# => adds flash
#######################################
FW_LDFLAGS =
include ./FW/main_fw.mk
####################################### PATCH> #######################################
"""

patch_main_ = """
    /* USER CODE BEGIN 3 */
    int main_fw(void);
    main_fw();
    // patched at %s
  }
  /* USER CODE END 3 */
""" % (time.ctime())

pattern_code3 = re.compile(r"/[*] USER CODE BEGIN 3 [*]/(.*?)/[*] USER CODE END 3 [*]/", re.MULTILINE | re.DOTALL)

def patch_makefile():
    with open("../Makefile", "r+") as file:
        content = file.read()
        lines = content.splitlines()
    
        # check if already patched
        if "include ./FW/main_fw.mk" in content:
            # already patched
            print("this file is already patched ... stop")
            return
    
        # create backup
        with open("../Makefile.backup", "w") as backup:
            backup.write(content)
    
        cnt = 0
        for line in lines:
            cnt += 1
            if line.startswith("PREFIX = "):
                break
    
        cnt -= 1    # line 1 => list index 0
        while cnt > 0:
            cnt -= 1
            if not lines[cnt].startswith("#"):
                break
    
        txt = '\n'.join(lines[0:cnt]) + patch_makefile_ + "\n".join(lines[cnt:])
        txt += "\n# this file has been patched at %s\n\n" % (time.ctime())
    
        file.seek(0)
        file.write(txt)

def patch_main():
    with open("../Core/Src/main.c", "r+") as file:
        content = file.read()

        # create backup
        with open("../Core/Src/main.c.backup", "w") as backup:
            backup.write(content)
    
        txt = pattern_code3.sub(patch_main_, content)
        file.seek(0)
        file.write(txt)

#
if __name__ == "__main__":
    patch_makefile()
    patch_main()



