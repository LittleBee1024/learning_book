import vCommon from "../common/index.js"

const vAbout = {
   components : {
      'icommon': vCommon
   },
   template: `
      <div class='vabout'>
         <el-row :gutter="20">
            <el-col class="grid-content bg-purple" :span="8">
               <icommon />
            </el-col>
            <el-col class="grid-content bg-purple-dark" :span="8">
               <icommon />
            </el-col>
            <el-col class="grid-content bg-purple-light" :span="8">
               <icommon />
            </el-col>
         </el-row>
      </div>
   `
}

export default vAbout;
