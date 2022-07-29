import vHeader from './header/index.js'
import vFooter from './footer/index.js'
import vHome from './home/index.js'

const vRoot = {
   data() {
      return {
         title: "Demo Header",
         footer: "Demo Footer"
      }
   },
   components: {
      'vheader': vHeader,
      'vhome': vHome,
      'vfooter': vFooter
   },
   template: `
      <div class='vroot'>
         <el-container>
            <el-header>
               <vheader :title="title" />
            </el-header>
            <el-main>
               <vhome />
            </el-main>
            <el-footer>
               <vfooter :footer="footer"/>
            </el-footer>
         </el-container>
      </div>
   `
}

export default vRoot;
