const BlogPost = {
   props: {
      // 基础的类型检查 (`null` 和 `undefined` 值会通过任何类型验证)
      propA: Number,
      // 多个可能的类型
      propB: [String, Number],
      // 必填的字符串
      propC: {
         type: String,
         required: true
      },
      // 带有默认值的数字
      propD: {
         type: Number,
         default: 100
      },
      // 带有默认值的对象
      propE: {
         type: Object,
         // 对象或数组的默认值必须从一个工厂函数返回
         default() {
            return { message: 'hello' }
         }
      },
      // 自定义验证函数
      propF: {
         validator(value) {
            // 这个值必须与下列字符串中的其中一个相匹配
            return ['success', 'warning', 'danger'].includes(value)
         }
      },
      // 具有默认值的函数
      propG: {
         type: Function,
         // 与对象或数组的默认值不同，这不是一个工厂函数——这是一个用作默认值的函数
         default() {
            return 'Default function'
         }
      }
   },
   template: `
      <p>propA: {{ propA }}</p>
      <p>propB: {{ propB }}</p>
      <p>propC: {{ propC }}</p>
      <p>propD: {{ propD }}</p>
      <p>propE: {{ propE }}</p>
      <p>propF: {{ propF }}</p>
      <p>propG: {{ propG }}</p>
   `
}

const RootComponent = {
   data() {
      return {
         post: {
            propA: 1,
            propB: 'abc',
            propC: 'def',
            propD: 101,
            propE: { message: 'world' },
            propF: 'danger',
            propG: function () { return 'proG function' }
         }
      }
   }
}

const app = Vue.createApp(RootComponent)

app.component('blog-post', BlogPost)

const vm = app.mount('#app')

