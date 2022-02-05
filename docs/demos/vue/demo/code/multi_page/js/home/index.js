import vCommon from "../common/index.js"

const vHome = {
   components : {
      'icommon': vCommon
   },
   template: `
      <div class='vhome'>
         <el-row :gutter="20">
            <el-col class="grid-content bg-purple" :span="12">
               <icommon />
            </el-col>
            <el-col class="grid-content bg-purple-dark" :span="12">
               <icommon />
            </el-col>
         </el-row>
      </div>
   `
}

export default vHome;
